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

#include "Selectivities/Manager.h"
#include "Utilities/DoubleCompare.h"

// Namespaces
namespace isam {
namespace processes {

/**
 * Default Constructor
 */
MortalityEvent::MortalityEvent() {
  parameters_.RegisterAllowed(PARAM_LABEL);
  parameters_.RegisterAllowed(PARAM_TYPE);
  parameters_.RegisterAllowed(PARAM_CATEGORIES);
  parameters_.RegisterAllowed(PARAM_YEARS);
  parameters_.RegisterAllowed(PARAM_CATCHES);
  parameters_.RegisterAllowed(PARAM_U_MAX);
  parameters_.RegisterAllowed(PARAM_SELECTIVITIES);
  parameters_.RegisterAllowed(PARAM_PENALTY);

  model_ = Model::Instance();
}

/**
 * Validate our Mortality Event Process
 *
 * 1. Check for the required parameters
 * 2. Assign any remaining variables
 */
void MortalityEvent::Validate() {
  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_TYPE);
  CheckForRequiredParameter(PARAM_CATEGORIES);
  CheckForRequiredParameter(PARAM_YEARS);
  CheckForRequiredParameter(PARAM_CATCHES);
  CheckForRequiredParameter(PARAM_U_MAX);
  CheckForRequiredParameter(PARAM_SELECTIVITIES);

  // Assign Parameters
  vector<unsigned>            years;
  vector<double>              catches;

  label_              = parameters_.Get(PARAM_LABEL).GetValue<string>();
  category_names_     = parameters_.Get(PARAM_CATEGORIES).GetValues<string>();
  years               = parameters_.Get(PARAM_YEARS).GetValues<unsigned>();
  catches             = parameters_.Get(PARAM_CATCHES).GetValues<double>();
  u_max_              = parameters_.Get(PARAM_U_MAX).GetValue<double>(0.99);
  selectivity_names_  = parameters_.Get(PARAM_SELECTIVITIES).GetValues<string>();
  penalty_            = parameters_.Get(PARAM_PENALTY).GetValue<string>("");

  // Validate that our number of years and catches vectors are the same size
  if (years.size() != catches.size()) {
    Parameter parameter = parameters_.Get(PARAM_CATCHES);
    LOG_ERROR("At line " << parameter.line_number() << " in file " << parameter.file_name()
        << ": Number of catches provided does not match the number of years provided."
        << " Expected " << years.size() << " but got " << catches.size());
  }

  // Validate that the number of selectivities is the same as the number of categories
  if (category_names_.size() != selectivity_names_.size()) {
    Parameter parameter = parameters_.Get(PARAM_SELECTIVITIES);
    LOG_ERROR("At line " << parameter.line_number() << " in file " << parameter.file_name()
        << ": Number of selectivities provided does not match the number of categories provided."
        << " Expected " << category_names_.size() << " but got " << selectivity_names_.size());
  }

  // Validate: catches and years
  for(unsigned i = 0; i < years.size(); ++i) {
    if (catches_.find(years[i]) != catches_.end()) {
      LOG_ERROR(parameters_.location(PARAM_YEARS) << ": year " << years[i] << " has already been specified, please remove the duplicate");
    }

    catches_[years[i]] = catches[i];
  }

  // Validate u_max
  if (u_max_ < 0.0 || u_max_ > 1.0)
    LOG_ERROR(parameters_.location(PARAM_U_MAX) << ": u_max must be between 0.0 and 1.0 (inclusive). Value defined was " << u_max_);
}

/**
 * Build the runtime relationships required
 * - Build partition reference
 */
void MortalityEvent::Build() {
  partition_ = accessor::CategoriesPtr(new isam::partition::accessors::Categories(category_names_));

  for (string label : selectivity_names_) {
    SelectivityPtr selectivity = selectivities::Manager::Instance().GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR(parameters_.location(PARAM_SELECTIVITIES) << ": selectivity " << label << " does not exist. Have you defined it?");

    selectivities_.push_back(selectivity);
  }
}

/**
 *
 */
void MortalityEvent::Execute() {
  if (catches_.find(model_->current_year()) == catches_.end())
    return;

  /**
   * Work our how much of the stock is vulnerable
   */
  double vulnerable = 0.0;
  unsigned i = 0;
  for (auto iterator = partition_->Begin(); iterator != partition_->End(); ++iterator, ++i) {
    unsigned min_age = (*iterator)->min_age_;

    for (unsigned offset = 0; offset < (*iterator)->data_.size(); ++offset) {
      double temp = (*iterator)->data_[offset] * selectivities_[i]->GetResult(min_age + offset);

      vulnerable += temp;
      vulnerable_[(*iterator)->name_][min_age + offset] = temp;
    }
  }

  /**
   * Work out the exploitation rate to remove (catch/vulnerable)
   */
  double exploitation = catches_[model_->current_year()] / utilities::doublecompare::ZeroFun(vulnerable);
  if (exploitation > u_max_) {
    exploitation = u_max_;
    // TODO: Trigger penalty

  } else if (exploitation < 0.0) {
    exploitation = 0.0;
  }
  LOG_INFO("year: " << model_->current_year() << "; exploitation: " << exploitation);

  /**
   * Remove the stock now. The amount to remove is
   * vulnerable * exploitation
   */
  for (auto iterator = partition_->Begin(); iterator != partition_->End(); ++iterator) {
    unsigned min_age = (*iterator)->min_age_;

    for (unsigned offset = 0; offset < (*iterator)->data_.size(); ++offset) {
      double temp = vulnerable_[(*iterator)->name_][min_age + offset] * exploitation;
      (*iterator)->data_[offset] -= temp;
    }
  }
}


} /* namespace processes */
} /* namespace isam */
