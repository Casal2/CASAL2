/**
 * @file MaturationRate.cpp
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
#include "MaturationRate.h"

#include "Categories/Categories.h"
#include "Selectivities/Manager.h"

// Namespaces
namespace isam {
namespace processes {

/**
 * Default Constructor
 */
MaturationRate::MaturationRate() {
  LOG_TRACE();

  parameters_.RegisterAllowed(PARAM_LABEL);
  parameters_.RegisterAllowed(PARAM_TYPE);
  parameters_.RegisterAllowed(PARAM_FROM);
  parameters_.RegisterAllowed(PARAM_TO);
  parameters_.RegisterAllowed(PARAM_PROPORTIONS);
  parameters_.RegisterAllowed(PARAM_SELECTIVITIES);
}

/**
 * Validate our Maturation Rate process
 *
 * 1. Check for the required parameters
 * 2. Assign variables from our parameters
 */
void MaturationRate::Validate() {
  // Check for required parameters
  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_TYPE);
  CheckForRequiredParameter(PARAM_FROM);
  CheckForRequiredParameter(PARAM_TO);
  CheckForRequiredParameter(PARAM_PROPORTIONS);
  CheckForRequiredParameter(PARAM_SELECTIVITIES);

  // Assign local variables
  label_                = parameters_.Get(PARAM_LABEL).GetValue<string>();
  from_category_names_  = parameters_.Get(PARAM_FROM).GetValues<string>();
  to_category_names_    = parameters_.Get(PARAM_TO).GetValues<string>();
  proportions_          = parameters_.Get(PARAM_PROPORTIONS).GetValues<double>();
  selectivity_names_    = parameters_.Get(PARAM_SELECTIVITIES).GetValues<string>();

  // Validate the from and to vectors are the same size
  if (from_category_names_.size() != to_category_names_.size()) {
    Parameter parameter = parameters_.Get(PARAM_TO);
    LOG_ERROR("At line " << parameter.line_number() << " in file " << parameter.file_name()
        << ": Number of to categories provided does not match the number of from categories provided."
        << " Expected " << from_category_names_.size() << " but got " << to_category_names_.size());
  }

  // Validate the to category and proportions vectors are the same size
  if (to_category_names_.size() != proportions_.size()) {
    Parameter parameter = parameters_.Get(PARAM_PROPORTIONS);
    LOG_ERROR("At line " << parameter.line_number() << " in file " << parameter.file_name()
        << ": Number of proportions provided does not match the number of 'to' categories provided."
        << " Expected " << to_category_names_.size() << " but got " << proportions_.size());
  }

  // Validate the number of selectivities matches the number of proportions
  if (proportions_.size() != selectivity_names_.size()) {
    Parameter parameter = parameters_.Get(PARAM_SELECTIVITIES);
    LOG_ERROR("At line " << parameter.line_number() << " in file " << parameter.file_name()
        << ": Number of selectivities provided does not match the number of proportions provided."
        << " Expected " << proportions_.size() << " but got " << selectivity_names_.size());
  }

  // Validate that each from and to category have the same age range.
  CategoriesPtr categories = Categories::Instance();
  for (unsigned i = 0; i < from_category_names_.size(); ++i) {
    if (categories->min_age(from_category_names_[i]) != categories->min_age(to_category_names_[i])) {
      LOG_ERROR(parameters_.location(PARAM_FROM) << ": Category " << from_category_names_[i] << " does not"
          << " have the same age range as the 'to' category " << to_category_names_[i]);
    }

    if (categories->max_age(from_category_names_[i]) != categories->max_age(to_category_names_[i])) {
      LOG_ERROR(parameters_.location(PARAM_FROM) << ": Category " << from_category_names_[i] << " does not"
          << " have the same age range as the 'to' category " << to_category_names_[i]);
    }
  }
}

/**
 * Build any runtime relationships this class needs.
 * - Build the partition accessors
 */
void MaturationRate::Build() {
  from_partition_ = accessor::CategoriesPtr(new partition::accessors::Categories(from_category_names_));
  to_partition_   = accessor::CategoriesPtr(new partition::accessors::Categories(to_category_names_));

  for(string label : selectivity_names_) {
    SelectivityPtr selectivity = selectivities::Manager::Instance().GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR(parameters_.location(PARAM_SELECTIVITIES) << ": Selectivity " << label << " does not exist. Have you defined it?");
    selectivities_.push_back(selectivity);
  }
}

/**
 * Execute our maturation rate process.
 */
void MaturationRate::Execute() {

  auto from_iter     = from_partition_->Begin();
  auto to_iter       = to_partition_->Begin();
  unsigned min_age   = (*from_iter)->min_age_;
  double amount      = 0.0;

  for (unsigned i = 0; from_iter != from_partition_->End() && to_iter != to_partition_->End(); ++from_iter, ++to_iter, ++i) {
    SelectivityPtr selectivity = selectivities_[i];

    for (unsigned offset = 0; offset < (*from_iter)->data_.size(); ++offset) {
      amount = proportions_[i] * selectivity->GetResult(min_age + offset) * (*from_iter)->data_[offset];
      (*from_iter)->data_[offset] -= amount;
      (*to_iter)->data_[offset] += amount;
    }
  }
}

} /* namespace processes */
} /* namespace isam */
