/**
 * @file Category.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 18/02/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 */

// headers
#include "Category.h"

#include "Model/Model.h"

// namespaces
namespace niwa {
namespace partition {

/**
 * This method will populate the length data on the partition category object.
 * This is required to be called by any object that wants to update the length
 * data from the age data.
 */
void Category::UpdateMeanLengthData() {
  if (!age_length_)
    return;

  unsigned year = Model::Instance()->current_year();

  for (unsigned age = min_age_; age <= max_age_; ++age)
    weight_per_[age] = age_length_->mean_length(year, age);
}

/**
 * This method will populate the age data from the length data. This is required
 * to transfer any changes in the length partition back to the age partition.
 */
void Category::ConvertLengthDataToAge() {
  data_.clear();

  for (auto iter : length_data_) {
    Double total = 0;
    for (auto x : iter.second)
      total += x.second;
    data_.push_back(total);
  }
}

/**
 *
 */
void Category::ConvertAgeDataToLength() {
  if (!age_length_key_)
    return;

  length_data_.clear();

  map<unsigned,map<unsigned, Double>>& lookup_table = age_length_key_->lookup_table();
  for (unsigned i = 0; i < data_.size(); ++i) {
    unsigned age = min_age_ + i;
    Double amount = data_[i];
    unsigned bin = 0;
    for (auto iter : lookup_table[age]) {
      length_data_[age][bin] = iter.second * amount;
      ++bin;
    }
  }

}


/**
 * This method will update the weight data with the number of fish and weight
 * per fish for use.
 */
void Category::UpdateMeanWeightData() {
  if (!age_length_)
    return;

  unsigned year = Model::Instance()->current_year();

  for (unsigned age = min_age_; age <= max_age_; ++age)
    weight_per_[age] = age_length_->mean_weight(year, age);
}

} /* namespace partitions */
} /* namespace niwa */
