/**
 * @file Cinitial.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 11/04/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */

#include "Cinitial.h"

#include "Categories/Categories.h"
#include "Model/Managers.h"
#include "Model/Model.h"
#include "TimeSteps/Manager.h"

namespace niwa {
namespace initialisationphases {

Cinitial::Cinitial(Model* model)
  : InitialisationPhase(model) {

  n_table_ = new parameters::Table(PARAM_N);

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "List of categories to use", "");
  parameters_.BindTable(PARAM_N, n_table_, "Table of data", "", false, false);

  RegisterAsEstimable(&n_);
}

/**
 * Destructor
 */
Cinitial::~Cinitial() {
  delete n_table_;
}

/**
 * Validate the parameters passed in from the configuration file
 */
void Cinitial::DoValidate() {
	min_age_ = model_->min_age();
	max_age_ = model_->max_age();

  if (max_age_ < min_age_)
    LOG_ERROR_P(PARAM_MIN_AGE) << "(" << min_age_ << ") cannot be less than the max age(" << max_age_ << ")";

  column_count_ = (max_age_ - min_age_) + 2;

  /**
   * Convert the string values to doubles and load them in to a table.
   */
  vector<vector<string>>& data = n_table_->data();
  unsigned row_number = 1;
  for (auto row : data) {
    if (row.size() != column_count_)
      LOG_ERROR_P(PARAM_N) << "the " << row_number << "the row has " << row.size() << " values but " << column_count_ << " values are expected";
    if (n_.find(row[0]) != n_.end())
      LOG_ERROR_P(PARAM_N) << "the category " << row[0] << " is defined more than once. You can only define a category once";


    for (unsigned i = 1; i < row.size(); ++i) {
      Double temp = Double();
      if (!utilities::To<Double>(row[i], temp))
        LOG_ERROR_P(PARAM_N) << "value (" << row[i] << ") in row " << row_number << " is not a valid numeric";
      n_[row[0]].push_back(temp);
    }
    row_number++;
  }
}

/**
 * Build runtime relationships between this object and other objects.
 * Build any data objects that need to be built.
 */
void Cinitial::DoBuild() {
  partition_ = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, category_labels_));
}

/**
 * Execute this process
 */
void Cinitial::Execute() {

  map<string, vector<Double>> category_by_age_total;

  auto partition_iter = partition_->Begin();
  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter) {
    category_by_age_total[category_labels_[category_offset]].assign((max_age_ - min_age_ + 1), 0.0);
    /**
     * Loop through the  combined categories building up the total abundance for each category label
     */
    auto category_iter = partition_iter->begin();
    for (; category_iter != partition_iter->end(); ++category_iter) {
      for (unsigned data_offset = 0; data_offset < (max_age_ - min_age_ + 1); ++data_offset) {
        unsigned age_offset = (*category_iter)->min_age_ - min_age_;
        // if this category min_age occurs after model min age ignore this age
        if (data_offset < age_offset)
          continue;
        category_by_age_total[category_labels_[category_offset]][data_offset] += (*category_iter)->data_[data_offset - age_offset];
      }
    }
  }

  // loop through the category_labels and calculate the cinitial factor, which is the n_ / col_sums
  map<string, vector<Double>> category_by_age_factor;

  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset) {
    category_by_age_factor[category_labels_[category_offset]].assign((max_age_ - min_age_ + 1), 0.0);
    for (unsigned data_offset = 0; data_offset < (max_age_ - min_age_ + 1); ++data_offset) {

      if (category_by_age_total[category_labels_[category_offset]][data_offset] == 0.0)
        category_by_age_factor[category_labels_[category_offset]][data_offset] = 1.0;
      else
        category_by_age_factor[category_labels_[category_offset]][data_offset] = n_[category_labels_[category_offset]][data_offset]
            / category_by_age_total[category_labels_[category_offset]][data_offset];
    }
  }
  // Now loop through the combined categories multiplying each category by the factory
  // from the combined group it belongs to
  partition_iter = partition_->Begin();
  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter) {
    /**
     * Loop through the  combined categories building up the total abundance for each category label
     */
    auto category_iter = partition_iter->begin();
    for (; category_iter != partition_iter->end(); ++category_iter) {
      for (unsigned data_offset = 0; data_offset < (max_age_ - min_age_ + 1); ++data_offset) {
        unsigned age_offset = (*category_iter)->min_age_ - min_age_;
        // if this category min_age occurs after model min age ignore this age
        if (data_offset < age_offset)
          continue;
        (*category_iter)->data_[data_offset - age_offset] *= category_by_age_factor[category_labels_[category_offset]][data_offset];
      }
    }
  }
}

} /* namespace initialisationphases */
} /* namespace niwa */
