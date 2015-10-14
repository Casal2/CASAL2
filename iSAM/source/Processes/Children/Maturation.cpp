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
#include "Selectivities/Selectivity.h"

// namespaces
namespace niwa {
namespace processes {

/**
 * default constructor
 */
Maturation::Maturation(Model* model)
  : Process(model),
    from_partition_(model),
    to_partition_(model) {
  parameters_.Bind<string>(PARAM_FROM, &from_category_names_, "List of categories to mature from", "");
  parameters_.Bind<string>(PARAM_TO, &to_category_names_, "List of categories to mature too", "");
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_names_, "List of selectivities to use for maturation", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years to be associated with rates", "");
  parameters_.Bind<Double>(PARAM_RATES, &rates_, "The rates to mature for each year", "");

  RegisterAsEstimable(PARAM_RATES, &rates_by_years_);

  model_ = Model::Instance();
  process_type_ = ProcessType::kMaturation;
  partition_structure_ = PartitionStructure::kAge;
}

/**
 * validate the values from the configuration file
 */
void Maturation::DoValidate() {
  if (selectivity_names_.size() == 1)
    selectivity_names_.assign(from_category_names_.size(), selectivity_names_[0]);

  // Validate Categories
  niwa::Categories* categories = model_->categories();
  for (const string& label : from_category_names_) {
    if (!categories->IsValid(label))
      LOG_ERROR_P(PARAM_FROM) << ": category " << label << " does not exist. Have you defined it?";
  }
  for(const string& label : to_category_names_) {
    if (!categories->IsValid(label))
      LOG_ERROR_P(PARAM_TO) << ": category " << label << " does not exist. Have you defined it?";
  }

  // Validate the from and to vectors are the same size
  if (from_category_names_.size() != to_category_names_.size()) {
    LOG_ERROR_P(PARAM_TO)
        << ": Number of 'to' categories provided does not match the number of 'from' categories provided."
        << " Expected " << from_category_names_.size() << " but got " << to_category_names_.size();
  }

  // Validate that each from and to category have the same age range.
  for (unsigned i = 0; i < from_category_names_.size(); ++i) {
    if (categories->min_age(from_category_names_[i]) != categories->min_age(to_category_names_[i])) {
      LOG_ERROR_P(PARAM_FROM) << ": Category " << from_category_names_[i] << " does not"
          << " have the same age range as the 'to' category " << to_category_names_[i];
    }

    if (categories->max_age(from_category_names_[i]) != categories->max_age(to_category_names_[i])) {
      LOG_ERROR_P(PARAM_FROM) << ": Category " << from_category_names_[i] << " does not"
          << " have the same age range as the 'to' category " << to_category_names_[i];
    }
  }

  // Validate rates and years are the same length
  if (rates_.size() != years_.size())
    LOG_ERROR_P(PARAM_RATES) << " number (" << rates_.size() << ") does not match the number of years (" << years_.size() << ") provided";
  for (unsigned i = 0; i < years_.size(); ++i)
    rates_by_years_[years_[i]] = rates_[i];

}

/**
 * Build any runtime relationships this class needs.
 * - Build the partition accessors
 * - Verify the selectivities are valid
 * - Get pointers to the selectivities
 */
void Maturation::DoBuild() {
  LOG_TRACE();

  from_partition_.Init(from_category_names_);
  to_partition_.Init(to_category_names_);

  for(string label : selectivity_names_) {
    Selectivity* selectivity = model_->managers().selectivity()->GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR_P(PARAM_SELECTIVITIES) << ": Selectivity " << label << " does not exist. Have you defined it?";
    selectivities_.push_back(selectivity);
  }
}

/**
 * Execute our maturation rate process.
 */
void Maturation::DoExecute() {
  auto from_iter     = from_partition_.begin();
  auto to_iter       = to_partition_.begin();
  Double amount      = 0.0;

  unsigned current_year = model_->current_year();
  Double rate = rates_by_years_[current_year];
  // if year is missing for projection then we grab the last one
  if (rates_by_years_.find(current_year) == rates_by_years_.end())
    rate = rates_by_years_.rbegin()->second;

  for (unsigned i = 0; from_iter != from_partition_.end() && to_iter != to_partition_.end(); ++from_iter, ++to_iter, ++i) {
    unsigned min_age   = (*from_iter)->min_age_;

    for (unsigned offset = 0; offset < (*from_iter)->data_.size(); ++offset) {
      amount = rate * selectivities_[i]->GetResult(min_age + offset);
      (*from_iter)->data_[offset] -= amount;
      (*to_iter)->data_[offset] += amount;
    }
  }
}

} /* namespace processes */
} /* namespace niwa */
