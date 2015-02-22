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
void Category::UpdateLengthData() {
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
void Category::UpdateAgeData() {

}

/**
 * This method will update the weight data with the number of fish and weight
 * per fish for use.
 */
void Category::UpdateWeightData() {
  if (!age_length_)
    return;

  unsigned year = Model::Instance()->current_year();

  for (unsigned age = min_age_; age <= max_age_; ++age)
    weight_per_[age] = age_length_->mean_weight(year, age);
}

} /* namespace partitions */
} /* namespace niwa */
