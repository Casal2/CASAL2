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

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_names_, "List of categories", "");
  parameters_.Bind<double>(PARAM_M, &m_, "Mortality rates", "");
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_names_, "Selectivities", "");

  RegisterAsEstimable(PARAM_M, &m_);
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
void MortalityConstantRate::DoValidate() {
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
  for (Double m : m_) {
    if (m < 0.0 || m > 1.0)
      LOG_ERROR(parameters_.location(PARAM_M) << ": m value " << AS_DOUBLE(m) << " must be between 0.0 and 1.0 (inclusive)");
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
void MortalityConstantRate::DoBuild() {
  partition_.Init(category_names_);

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

  unsigned i = 0;
  for (auto category : partition_) {
    Double m = m_.size() > 1 ? m_[i] : m_[0];

    unsigned j = 0;
    for (Double& data : category->data_) {
      data -= data * (1-exp(-selectivities_[i]->GetResult(category->min_age_ + j)  * m));
      ++j;
    }

    ++i;
  }

//  if (mortality_rates_.size() == 0) {
//    auto iter = partition_.begin();
//    mortality_rates_.resize(category_names_.size());
//    for (unsigned i = 0; iter != partition_.end(); ++iter, ++i) {
//      unsigned min_age = (*iter)->min_age_;
//      Double m = m_.size() > 1 ? m_[i] : m_[0];
//
//      for (unsigned j = 0; j < (*iter)->data_.size(); ++j) {
//        mortality_rates_[i].push_back(1-exp(-selectivities_[i]->GetResult(min_age + j) * m));
//      }
//    }
//  }
//
//  auto iterator = partition_.begin();
//  for (unsigned i = 0; iterator != partition_.end(); ++iterator, ++i) {
//    for (unsigned j = 0; j < (*iterator)->data_.size(); ++j) {
//      (*iterator)->data_[j] -= (*iterator)->data_[j] * mortality_rates_[i][j];
//    }
//  }
}

/**
 * Reset the Mortality Process
 */
void MortalityConstantRate::DoReset() {
  mortality_rates_.clear();
}

} /* namespace processes */
} /* namespace isam */
