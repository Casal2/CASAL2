/**
 * @file Maturation.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 11/09/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "Maturation.h"

#include "Categories/Categories.h"
#include "Selectivities/Manager.h"

// namespaces
namespace isam {
namespace processes {

/**
 * default constructor
 */
Maturation::Maturation() {
  LOG_TRACE();

  parameters_.RegisterAllowed(PARAM_LABEL);
  parameters_.RegisterAllowed(PARAM_TYPE);
  parameters_.RegisterAllowed(PARAM_FROM);
  parameters_.RegisterAllowed(PARAM_TO);
  parameters_.RegisterAllowed(PARAM_SELECTIVITIES);
  parameters_.RegisterAllowed(PARAM_YEARS);
  parameters_.RegisterAllowed(PARAM_RATES);

  model_ = Model::Instance();
}

/**
 * validate the values from the configuration file
 */
void Maturation::Validate() {
  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_TYPE);
  CheckForRequiredParameter(PARAM_FROM);
  CheckForRequiredParameter(PARAM_TO);
  CheckForRequiredParameter(PARAM_RATES);
  CheckForRequiredParameter(PARAM_YEARS);
  CheckForRequiredParameter(PARAM_SELECTIVITIES);

  label_                = parameters_.Get(PARAM_LABEL).GetValue<string>();
  from_category_names_  = parameters_.Get(PARAM_FROM).GetValues<string>();
  to_category_names_    = parameters_.Get(PARAM_TO).GetValues<string>();
  rates_                = parameters_.Get(PARAM_RATES).GetValues<Double>();
  years_                = parameters_.Get(PARAM_YEARS).GetValues<unsigned>();
  selectivity_names_    = parameters_.Get(PARAM_SELECTIVITIES).GetValues<string>();

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

  // Validate rates and years are the same length
  if (rates_.size() != years_.size())
    LOG_ERROR(parameters_.location(PARAM_RATES) << " number (" << rates_.size() << ") does not match the number of years (" << years_.size() << ") provided");
  for (unsigned i = 0; i < years_.size(); ++i)
    year_rates_[years_[i]] = rates_[i];

}

/**
 * Build any runtime relationships this class needs.
 * - Build the partition accessors
 * - Verify the selectivities are valid
 * - Get pointers to the selectivities
 */
void Maturation::Build() {
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
void Maturation::Execute() {
  auto from_iter     = from_partition_->Begin();
  auto to_iter       = to_partition_->Begin();
  Double amount      = 0.0;

  unsigned current_year = model_->current_year();

  for (unsigned i = 0; from_iter != from_partition_->End() && to_iter != to_partition_->End(); ++from_iter, ++to_iter, ++i) {
    unsigned min_age   = (*from_iter)->min_age_;

    for (unsigned offset = 0; offset < (*from_iter)->data_.size(); ++offset) {
      amount = year_rates_[current_year] * selectivities_[i]->GetResult(min_age + offset);
      (*from_iter)->data_[offset] -= amount;
      (*to_iter)->data_[offset] += amount;
    }
  }
}

} /* namespace processes */
} /* namespace isam */
