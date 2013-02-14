/**
 * @file MortalityConstantRate.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 20/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "MortalityConstantRate.h"

#include "Categories/Categories.h"
#include "Selectivities/Manager.h"
#include "Utilities/Logging/Logging.h"

// Namespaces
namespace isam {
namespace processes {

/**
 * Default Constructor
 */
MortalityConstantRate::MortalityConstantRate() {
  LOG_TRACE();

  parameters_.RegisterAllowed(PARAM_LABEL);
  parameters_.RegisterAllowed(PARAM_TYPE);
  parameters_.RegisterAllowed(PARAM_CATEGORIES);
  parameters_.RegisterAllowed(PARAM_M);
  parameters_.RegisterAllowed(PARAM_SELECTIVITIES);
}

/**
 * Validate our Mortality Constant Rate process
 *
 * - Validate the required parameters
 * - Assign the label from the parameters
 * - Assign and validate remaining parameters
 * - Duplicate 'm' and 'selectivities' if only 1 vale specified
 * - Check m is between 0.0 and 1.0
 * - Check the categories are real
 */
void MortalityConstantRate::Validate() {
  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_TYPE);
  CheckForRequiredParameter(PARAM_CATEGORIES);
  CheckForRequiredParameter(PARAM_M);
  CheckForRequiredParameter(PARAM_SELECTIVITIES);

  // Assign and validate parameters
  label_              = parameters_.Get(PARAM_LABEL).GetValue<string>();
  category_names_     = parameters_.Get(PARAM_CATEGORIES).GetValues<string>();
  m_                  = parameters_.Get(PARAM_M).GetValues<double>();
  selectivity_names_  = parameters_.Get(PARAM_SELECTIVITIES).GetValues<string>();

  if (m_.size() == 1)
    m_.assign(category_names_.size(), m_[0]);
  if (selectivity_names_.size() == 1)
    selectivity_names_.assign(category_names_.size(), selectivity_names_[0]);

  if (m_.size() != category_names_.size()) {
    LOG_ERROR(parameters_.location(PARAM_M)
        << ": Number of Ms provided is not the same as the number of categories provided. Expected: "
        << category_names_.size()<< " but got " << m_.size());
  }

  if (selectivity_names_.size() != category_names_.size()) {
    LOG_ERROR(parameters_.location(PARAM_SELECTIVITIES)
        << ": Number of selectivities provided is not the same as the number of categories provided. Expected: "
        << category_names_.size()<< " but got " << selectivity_names_.size());
  }

  // Validate our Ms are between 1.0 and 0.0
  for (double m : m_) {
    if (m < 0.0 || m > 1.0)
      LOG_ERROR(parameters_.location(PARAM_M) << ": m value " << m << " must be between 0.0 and 1.0 (inclusive)");
  }

  // Check categories are real
  for (const string& label : category_names_) {
    if (!Categories::Instance()->IsValid(label))
      LOG_ERROR(parameters_.location(PARAM_CATEGORIES) << ": category " << label << " does not exist. Have you defined it?");
  }
}

/**
 * Build any runtime relationships
 * - Build the partition accessor
 * - Build our list of selectivities
 */
void MortalityConstantRate::Build() {
  partition_ = accessor::CategoriesPtr(new partition::accessors::Categories(category_names_));

  for (string label : selectivity_names_) {
    SelectivityPtr selectivity = selectivities::Manager::Instance().GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR(parameters_.location(PARAM_SELECTIVITIES) << ": selectivity " << label << " does not exist. Have you defined it?");

    selectivities_.push_back(selectivity);
  }
}

/**
 * Execute the process
 */
void MortalityConstantRate::Execute() {
  Double amount     = 0.0;
  unsigned min_age  = 0;

  auto iterator = partition_->Begin();
  for (unsigned i = 0; iterator != partition_->End(); ++iterator, ++i) {
    min_age = (*iterator)->min_age_;

    for (unsigned offset = 0; offset < (*iterator)->data_.size(); ++offset) {
      double mortality_rate = 1-exp(-(selectivities_[i]->GetResult(min_age + offset) * m_[i]));
      amount = (*iterator)->data_[offset] * mortality_rate;
      (*iterator)->data_[offset] -= amount;
    }
  }
}

} /* namespace processes */
} /* namespace isam */
