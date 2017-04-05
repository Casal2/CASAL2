/**
 * @file MortalityEvent.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 21/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "MortalityEvent.h"

#include "Categories/Categories.h"
#include "Penalties/Manager.h"
#include "Selectivities/Manager.h"
#include "Utilities/DoubleCompare.h"

// Namespaces
namespace niwa {
namespace processes {

/**
 * Default Constructor
 */
MortalityEvent::MortalityEvent(Model* model)
  : Process(model),
    partition_(model) {
  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "Categories", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years in which to apply the mortality process", "");
  parameters_.Bind<Double>(PARAM_CATCHES, &catches_, "The number of removals (catches) to apply for each year", "");
  parameters_.Bind<Double>(PARAM_U_MAX, &u_max_, "Maximum exploitation rate ($Umax$)", "", 0.99);
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_names_, "List of selectivities", "");
  parameters_.Bind<bool>(PARAM_INITIALISATION, &apply_in_initialisation_, "Apply the first value of the catches in the intialisation phase", "", false);
  parameters_.Bind<string>(PARAM_PENALTY, &penalty_name_, "The label of the penalty to apply if the total number of removals cannot be taken", "", "");

  RegisterAsEstimable(PARAM_U_MAX, &u_max_);
  RegisterAsEstimable(PARAM_CATCHES, &catch_years_);

  process_type_ = ProcessType::kMortality;
  partition_structure_ = PartitionStructure::kAge;
}

/**
 * Validate our Mortality Event Process
 *
 * 1. Check for the required parameters
 * 2. Assign any remaining variables
 */
void MortalityEvent::DoValidate() {
  category_labels_ = model_->categories()->ExpandLabels(category_labels_, parameters_.Get(PARAM_CATEGORIES));

  // Validate that our number of years_ and catches_ vectors are the same size
  if (years_.size() != catches_.size()) {
    LOG_ERROR_P(PARAM_CATCHES)
        << ": Number of catches_ provided does not match the number of years_ provided."
        << " Expected " << years_.size() << " but got " << catches_.size();
  }

  // Validate that the number of selectivities is the same as the number of categories
  if (category_labels_.size() != selectivity_names_.size()) {
    LOG_ERROR_P(PARAM_SELECTIVITIES)
        << " Number of selectivities provided does not match the number of categories provided."
        << " Expected " << category_labels_.size() << " but got " << selectivity_names_.size();
  }

  // Validate: catches_ and years_
  for(unsigned i = 0; i < years_.size(); ++i) {
    if (catch_years_.find(years_[i]) != catch_years_.end()) {
      LOG_ERROR_P(PARAM_YEARS) << " year " << years_[i] << " has already been specified, please remove the duplicate";
    }

    catch_years_[years_[i]] = catches_[i];
  }

  // Validate u_max
  if (u_max_ < 0.0 || u_max_ > 1.0)
    LOG_ERROR_P(PARAM_U_MAX) << ": u_max must be between 0.0 and 1.0 (inclusive). Value defined was " << AS_DOUBLE(u_max_);
}

/**
 * Build the runtime relationships required
 * - Build partition reference
 */
void MortalityEvent::DoBuild() {
  partition_.Init(category_labels_);

  for (string label : selectivity_names_) {
    Selectivity* selectivity = model_->managers().selectivity()->GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR_P(PARAM_SELECTIVITIES) << ": selectivity " << label << " does not exist. Have you defined it?";

    selectivities_.push_back(selectivity);
  }

  if (penalty_name_ != "") {
    penalty_ = model_->managers().penalty()->GetProcessPenalty(penalty_name_);
    if (!penalty_) {
      LOG_ERROR_P(PARAM_PENALTY) << ": penalty " << penalty_name_ << " does not exist. Have you defined it?";
    }
  }
}

/**
 * Execute our mortality event object.
 *
 */
void MortalityEvent::DoExecute() {
  LOG_TRACE();
  if (catch_years_.find(model_->current_year()) == catch_years_.end())
    return;

  if (((model_->state() == State::kInitialise) & apply_in_initialisation_) | ((model_->state() != State::kInitialise) & !apply_in_initialisation_)) {
    /**
     * Work our how much of the stock is vulnerable
     */
    Double vulnerable = 0.0;
    unsigned i = 0;
    for (auto categories : partition_) {
      unsigned j = 0;
      for (Double& data : categories->data_) {
        Double local_vulnerable = data * selectivities_[i]->GetResult(categories->min_age_ + j, categories->age_length_);
        vulnerable += local_vulnerable;
        vulnerable_[categories->name_][categories->min_age_ + j] = local_vulnerable;
        ++j;
      }

      ++i;
    }
    /**
     * Work out the exploitation rate to remove (catch/vulnerable)
     */
    Double exploitation = 0;
    if(model_->state() == State::kInitialise) {
      LOG_FINEST() << "vulnerable biomass = " << vulnerable << " catch = " << catch_years_[years_.front()];
      exploitation = catch_years_[years_.front()] / utilities::doublecompare::ZeroFun(vulnerable);
    } else {
      LOG_FINEST() << "vulnerable biomass = " << vulnerable << " catch = " << catch_years_[model_->current_year()];
      exploitation = catch_years_[model_->current_year()] / utilities::doublecompare::ZeroFun(vulnerable);
    }
    if (exploitation > u_max_) {
      exploitation = u_max_;
      if (penalty_)
        penalty_->Trigger(label_, catch_years_[model_->current_year()], vulnerable*u_max_);

    } else if (exploitation < 0.0) {
      exploitation = 0.0;
    }
    LOG_FINEST() << "year: " << model_->current_year() << "; exploitation: " << AS_DOUBLE(exploitation);

    /**
     * Remove the stock now. The amount to remove is
     * vulnerable * exploitation
     */
    for (auto categories : partition_) {
      unsigned offset = 0;
      for (Double& data : categories->data_) {
        data -= vulnerable_[categories->name_][categories->min_age_ + offset] * exploitation;
        offset++;
      }
    }
  }
}

} /* namespace processes */
} /* namespace niwa */
