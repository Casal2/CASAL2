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
 * - Check for the required parameters
 * - Assign variables from our parameters
 * - Verify the categories are real
 * - If proportions or selectivities only has 1 element specified
 *   add more elements until they match number of categories
 * - Verify vector lengths are all the same
 * - Verify categories From->To have matching age ranges
 * - Check all proportions are between 0.0 and 1.0

 */
void MaturationRate::Validate() {
  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_TYPE);
  CheckForRequiredParameter(PARAM_FROM);
  CheckForRequiredParameter(PARAM_TO);
  CheckForRequiredParameter(PARAM_PROPORTIONS);
  CheckForRequiredParameter(PARAM_SELECTIVITIES);

  label_                = parameters_.Get(PARAM_LABEL).GetValue<string>();
  from_category_names_  = parameters_.Get(PARAM_FROM).GetValues<string>();
  to_category_names_    = parameters_.Get(PARAM_TO).GetValues<string>();
  proportions_          = parameters_.Get(PARAM_PROPORTIONS).GetValues<double>();
  selectivity_names_    = parameters_.Get(PARAM_SELECTIVITIES).GetValues<string>();

  if (proportions_.size() == 1)
    proportions_.assign(from_category_names_.size(), proportions_[0]);
  if (selectivity_names_.size() == 1)
    selectivity_names_.assign(from_category_names_.size(), selectivity_names_[0]);

  // Validate Categories
  isam::CategoriesPtr categories = isam::Categories::Instance();
  for (const string& label : from_category_names_) {
    if (!categories->IsValid(label))
      LOG_ERROR(parameters_.location(PARAM_FROM) << ": category " << label << " does not exist. Have you defined it?");
  }
  for(const string& label : to_category_names_) {
    if (!categories->IsValid(label))
      LOG_ERROR(parameters_.location(PARAM_TO) << ": category " << label << " does not exist. Have you defined it?");
  }

  // Validate the from and to vectors are the same size
  if (from_category_names_.size() != to_category_names_.size()) {
    LOG_ERROR(parameters_.location(PARAM_TO)
        << ": Number of 'to' categories provided does not match the number of 'from' categories provided."
        << " Expected " << from_category_names_.size() << " but got " << to_category_names_.size());
  }

  // Validate the to category and proportions vectors are the same size
  if (to_category_names_.size() != proportions_.size()) {
    LOG_ERROR(parameters_.location(PARAM_PROPORTIONS)
        << ": Number of proportions provided does not match the number of 'to' categories provided."
        << " Expected " << to_category_names_.size() << " but got " << proportions_.size());
  }

  // Validate the number of selectivities matches the number of proportions
  if (proportions_.size() != selectivity_names_.size()) {
    LOG_ERROR(parameters_.location(PARAM_SELECTIVITIES)
        << ": Number of selectivities provided does not match the number of proportions provided."
        << " Expected " << proportions_.size() << " but got " << selectivity_names_.size());
  }

  // Validate that each from and to category have the same age range.
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

  // Validate the proportions are between 0.0 and 1.0
  for (Double proportion : proportions_) {
    if (proportion < 0.0 || proportion > 1.0)
      LOG_ERROR(parameters_.location(PARAM_PROPORTIONS) << ": proportion " << proportion << " must be between 0.0 and 1.0 (inclusive)");
  }

  RegisterAsEstimable(PARAM_PROPORTIONS, proportions_);
}

/**
 * Build any runtime relationships this class needs.
 * - Build the partition accessors
 * - Verify the selectivities are valid
 * - Get pointers to the selectivities
 */
void MaturationRate::Build() {
  LOG_TRACE();

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
  Double amount      = 0.0;

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
