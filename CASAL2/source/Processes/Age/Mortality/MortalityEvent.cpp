/**
 * @file MortalityEvent.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 21/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "MortalityEvent.h"

#include "Categories/Categories.h"
#include "Penalties/Manager.h"
#include "Selectivities/Manager.h"
#include "Utilities/Math.h"

// Namespaces
namespace niwa {
namespace processes {
namespace age {

/**
 * Default constructor
 */
MortalityEvent::MortalityEvent(shared_ptr<Model> model) : Mortality(model), partition_(model) {
  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The categories", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years in which to apply the mortality process", "");
  parameters_.Bind<Double>(PARAM_CATCHES, &catches_, "The number of removals (catches) to apply for each year", "");
  parameters_.Bind<double>(PARAM_U_MAX, &u_max_, "The maximum exploitation rate ($U_{max}$)", "", 0.99)->set_range(0.0, 1.0);
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_names_, "The list of selectivities", "");
  parameters_.Bind<string>(PARAM_PENALTY, &penalty_name_, "The label of the penalty to apply if the total number of removals cannot be taken", "", "");

  RegisterAsAddressable(PARAM_CATCHES, &catch_years_);

  process_type_        = ProcessType::kMortality;
  partition_structure_ = PartitionType::kAge;
}

/**
 * Validate the Mortality Event Process
 *
 * 1. Check for the required parameters
 * 2. Assign any remaining variables
 */
void MortalityEvent::DoValidate() {
  // Validate that our number of years_ and catches_ vectors are the same size
  if (years_.size() != catches_.size()) {
    LOG_ERROR_P(PARAM_CATCHES) << ": The number of catches provided (" << catches_.size() << ") does not match the number of years provided (" << years_.size() << " ).";
  }

  // Validate that the number of selectivities is the same as the number of categories
  if (category_labels_.size() != selectivity_names_.size()) {
    LOG_ERROR_P(PARAM_SELECTIVITIES) << " The number of selectivities provided (" << selectivity_names_.size() << ") does not match the number of categories provided ("
                                     << category_labels_.size() << ").";
  }

  // Validate: catches_ and years_
  for (unsigned i = 0; i < years_.size(); ++i) {
    if (catch_years_.find(years_[i]) != catch_years_.end()) {
      LOG_ERROR_P(PARAM_YEARS) << " year '" << years_[i] << "' has already been specified";
    }

    catch_years_[years_[i]] = catches_[i];
  }

  // Validate u_max
  if (u_max_ < 0.0 || u_max_ > 1.0)
    LOG_ERROR_P(PARAM_U_MAX) << ": u_max (" << u_max_ << ") must be between 0.0 and 1.0 (inclusive).";
}

/**
 * Build the runtime relationships required
 * - Build partition reference
 */
void MortalityEvent::DoBuild() {
  partition_.Init(category_labels_);

  for (string label : selectivity_names_) {
    Selectivity* selectivity = model_->managers()->selectivity()->GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR_P(PARAM_SELECTIVITIES) << ": Selectivity label " << label << " was not found.";

    selectivities_.push_back(selectivity);
  }

  if (penalty_name_ != "") {
    penalty_ = model_->managers()->penalty()->GetProcessPenalty(penalty_name_);
    if (!penalty_) {
      LOG_ERROR_P(PARAM_PENALTY) << ": Penalty label " << penalty_name_ << " was not found.";
    }
  }

  // Pre allocate memory
  exploitation_.reserve(years_.size());
  actual_catches_.reserve(years_.size());
}

/**
 * Reset the runtime relationships
 */
void MortalityEvent::DoReset() {
  exploitation_.clear();
  actual_catches_.clear();
}

/**
 * Execute the mortality event object
 */
void MortalityEvent::DoExecute() {
  LOG_TRACE();
  auto current_year = model_->current_year();
  if (catch_years_.find(current_year) == catch_years_.end())
    return;

  if (model_->state() != State::kInitialise) {
    /**
     * Work our how much of the stock is vulnerable
     */
    Double   vulnerable = 0.0;
    unsigned i          = 0;
    for (auto categories : partition_) {
      unsigned j = 0;
      for (Double& data : categories->data_) {
        Double local_vulnerable = data * selectivities_[i]->GetAgeResult(categories->min_age_ + j, categories->age_length_);
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
    LOG_FINEST() << "vulnerable biomass = " << vulnerable << " catch = " << catch_years_[current_year];
    exploitation = catch_years_[current_year] / utilities::math::ZeroFun(vulnerable);

    if (exploitation > u_max_) {
      exploitation = u_max_;
      actual_catches_.push_back(vulnerable * u_max_);
      exploitation_.push_back(exploitation);
      if (penalty_)
        penalty_->Trigger(catch_years_[current_year], vulnerable * u_max_);

    } else {
      actual_catches_.push_back(catch_years_[current_year]);
      exploitation_.push_back(exploitation);
    }
    if (exploitation < 0.0) {
      LOG_CODE_ERROR() << "exploitation < 0.0 for process " << label_;
      exploitation = 0.0;
    }
    LOG_FINEST() << "year: " << current_year << "; exploitation: " << AS_DOUBLE(exploitation);

    /**
     * Remove the stock now. The amount to remove is
     * vulnerable * exploitation
     */
    Double removals = 0;

    for (auto categories : partition_) {
      unsigned offset = 0;
      for (Double& data : categories->data_) {
        removals = vulnerable_[categories->name_][categories->min_age_ + offset] * exploitation;
        data -= removals;
        offset++;
      }
    }
  }
}

/**
 * Fill the report cache
 * @description A method for reporting process information
 * @param cache a cache object to print to
 */
void MortalityEvent::FillReportCache(ostringstream& cache) {
  cache << "years: ";
  for (auto year : years_) cache << year << " ";
  cache << "\nactual_catches: ";
  for (auto removal : actual_catches_) cache << AS_DOUBLE(removal) << " ";
  cache << "\nexploitation_rate: ";
  for (auto exploit : exploitation_) cache << AS_DOUBLE(exploit) << " ";
  cache << REPORT_EOL;
}

/**
 * Fill the tabular report cache
 * @description A method for reporting tabular process information
 * @param cache a cache object to print to
 * @param first_run whether to print the header
 *
 */
void MortalityEvent::FillTabularReportCache(ostringstream& cache, bool first_run) {
  if (first_run) {
    for (auto year : years_) {
      cache << "actual_catches[" << label_ << "][" << year << "] ";
    }
    for (auto year : years_) {
      cache << "exploitation[" << label_ << "][" << year << "] ";
    }
    cache << REPORT_EOL;
  }

  for (auto removal : actual_catches_) cache << AS_DOUBLE(removal) << " ";
  for (auto exploit : exploitation_) cache << AS_DOUBLE(exploit) << " ";
  cache << REPORT_EOL;
}

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
