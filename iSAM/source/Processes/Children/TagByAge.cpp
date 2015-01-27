/**
 * @file TagByAge.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 26/01/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "TagByAge.h"

#include "Selectivities/Manager.h"
#include "Penalties/Manager.h"

// namespaces
namespace niwa {
namespace processes {

/**
 * Default constructor
 */
TagByAge::TagByAge() {
  process_type_ = ProcessType::kTaggingWithMortality;
  n_table_ = TablePtr(new parameters::Table(PARAM_N));

  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years", "");
  parameters_.Bind<string>(PARAM_FROM, &from_category_labels_, "From categories", "");
  parameters_.Bind<string>(PARAM_TO, &to_category_labels_, "To categories", "");
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "List of selectivity labels", "");
  parameters_.Bind<string>(PARAM_PENALTY, &penalty_label_, "Penalty label", "", "");
  parameters_.Bind<unsigned>(PARAM_MIN_AGE, &min_age_, "Min age to tag", "");
  parameters_.Bind<unsigned>(PARAM_MAX_AGE, &max_age_, "Max age to tag", "");
  parameters_.Bind<bool>(PARAM_AGE_PLUS, &age_plus_, "Use age plus or not", "");
  parameters_.Bind<Double>(PARAM_U_MAX, &u_max_, "Maximum exploitation of fish", "");
  parameters_.Bind<Double>(PARAM_INITIAL_MORTALITY, &initial_mortality_, "", "");
  parameters_.Bind<Double>(PARAM_CONSTANT_LOSS_RATE, &constant_loss_rate_, "Constant loss rate", "");
  parameters_.Bind<unsigned>(PARAM_TAG_LOSS_YEARS, &tag_loss_years_, "Tag loss years", "");
  parameters_.Bind<Double>(PARAM_TAG_LOSS_RATE, &tag_loss_rate_, "Tag loss rate", "");
  parameters_.BindTable(PARAM_N, n_table_, "Table of N data", "");
}

/**
 * Validate the parameters from the configuration file
 */
void TagByAge::DoValidate() {
  if (from_category_labels_.size() != to_category_labels_.size()) {
    LOG_ERROR(parameters_.location(PARAM_TO) << " number of values supplied (" << to_category_labels_.size()
        << ") does not match the number of from categories provided (" << from_category_labels_.size() << ")");
  }
  if (u_max_ <= 0.0 || u_max_ > 1.0)
    LOG_ERROR(parameters_.location(PARAM_U_MAX) << " (" << u_max_ << ") must be greater than 0.0 and less than 1.0");
  if (min_age_ < model_->min_age())
    LOG_ERROR(parameters_.location(PARAM_MIN_AGE) << " (" << min_age_ << ") is less than the model's minimum age (" << model_->min_age() << ")");
  if (max_age_ > model_->max_age())
    LOG_ERROR(parameters_.location(PARAM_MAX_AGE) << " (" << max_age_ << ") is greater than the model's maximum age (" << model_->max_age() << ")");

  unsigned age_spread = (max_age_ - min_age_) + 1;

  /**
   * Load our N data in to the map
   */
  vector<string> columns = n_table_->columns();
  if (columns.size() != age_spread + 1)
    LOG_ERROR(parameters_.location(PARAM_N) << " number of columns provided (" << columns.size() << ") does not match the model's age spread + 1 ("
        << (age_spread + 1) << ")");
  if (columns[0] != PARAM_YEAR)
    LOG_ERROR(parameters_.location(PARAM_N) << " first column label (" << columns[0] << ") provided must be 'year'");

  map<unsigned, unsigned> age_index;
  for (unsigned i = 1; i < columns.size(); ++i) {
    unsigned age = 0;
    if (!utilities::To<unsigned>(columns[i], age))
      LOG_ERROR("");
    age_index[age] = i;
  }

  // load our table data in to our map
  vector<vector<string>> data = n_table_->data();
  unsigned year = 0;
  Double n_value = 0.0;
  for (auto iter : data) {
    if (!utilities::To<unsigned>(iter[0], year))
      LOG_ERROR(parameters_.location(PARAM_N) << " value (" << iter[0] << ") is not a valid unsigned value that could be converted to a model year");
    for (unsigned i = 1; i < iter.size(); ++i) {
      if (!utilities::To<Double>(iter[i], n_value))
        LOG_ERROR(parameters_.location(PARAM_N) << " value (" << iter[i] << ") could not be converted to a double. Please ensure it's a numeric value");
      if (n_[year].size() == 0)
        n_[year].resize(age_spread, 0.0);
      n_[year][i - 1] = n_value;
    }
  }
}

/**
 * Build relationships between this object and others
 */
void TagByAge::DoBuild() {
  for (string label : selectivity_labels_) {
    SelectivityPtr selectivity = selectivities::Manager::Instance().GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR(parameters_.location(PARAM_SELECTIVITIES) << ": selectivity " << label << " does not exist. Have you defined it?");

    selectivities_.push_back(selectivity);
  }

  if (penalty_label_ != "") {
    penalty_ = penalties::Manager::Instance().GetProcessPenalty(penalty_label_);
    if (!penalty_) {
      LOG_ERROR(parameters_.location(PARAM_PENALTY) << ": penalty " << penalty_label_ << " does not exist. Have you defined it?");
    }
  }
}

/**
 * Execute this process
 */
void TagByAge::DoExecute() {

}

} /* namespace processes */
} /* namespace niwa */
