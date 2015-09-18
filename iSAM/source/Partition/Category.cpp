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

#include "Categories/Categories.h"
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
  CategoriesPtr categories = Categories::Instance();
  unsigned year = Model::Instance()->current_year();

  AgeLengthPtr age_length = categories->age_length(name_);
  age_length->BuildCV(year);
  for (unsigned age = min_age_; age <= max_age_; ++age)
    mean_length_per_[age] = age_length->mean_length(year, age);
}


/**
 * This method will update the weight data with the number of fish and weight
 * per fish for use.
 */

void Category::UpdateMeanWeightData() {
  CategoriesPtr categories = Categories::Instance();
  unsigned year = Model::Instance()->current_year();

  AgeLengthPtr age_length = categories->age_length(name_);
  age_length->BuildCV(year);
  for (unsigned age = min_age_; age <= max_age_; ++age)
    mean_weight_per_[age] = age_length->mean_weight(year, age);
}


/**
 * This method will populate the age data from the length data. This is required
 * to transfer any changes in the length partition back to the age partition.
 */
void Category::CollapseAgeLengthData() {
  data_.clear();

  for (auto age_row : age_length_matrix_) {
    Double total = 0;
    for (Double length_data : age_row)
      total += length_data;
    data_.push_back(total);
  }
}

/**
 * This method updates the numbers at age by length matrix for a category
 *
 * @param length_bins  vector of the length bins to map too
 * @param plus_grp whether the last length bin is a plus group
 * @param selectivity Selectivity Pointer
 */
void Category::UpdateAgeLengthData(const vector<Double>& length_bins, bool plus_grp, SelectivityPtr selectivity) {
  LOG_TRACE();

  CategoriesPtr categories = Categories::Instance();
  unsigned year = Model::Instance()->current_year();
  AgeLengthPtr age_length = categories->age_length(name_);
  if (!age_length)
    LOG_CODE_ERROR() << "if (!age_length) for category " << name_;
  age_length->BuildCV(year);
  age_length->DoAgeToLengthConversion(this, length_bins, plus_grp, selectivity);
}


/**
 * This method collapses the Numbers at length by age matrix to numbers at age for a category
 */
void Category::CollapseAgeLengthDataToLength() {
  if (age_length_matrix_.size() == 0)
    LOG_CODE_ERROR() << "if (age_length_matrix_.size() == 0)";

  length_data_.assign(age_length_matrix_[0].size(), 0.0);
  for (unsigned i = 0; i < age_length_matrix_.size(); ++i) {
    for (unsigned j = 0; j < age_length_matrix_[i].size(); ++j) {
      length_data_[j] += age_length_matrix_[i][j];
    }
  }
}


} /* namespace partitions */
} /* namespace niwa */
