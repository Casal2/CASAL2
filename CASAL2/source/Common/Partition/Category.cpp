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

#include "Age/AgeLengths/AgeLength.h"
#include "Common/LengthWeights/LengthWeight.h"
#include "Common/Categories/Categories.h"
#include "Common/Model/Model.h"
#include "Common/TimeSteps/Manager.h"
#include "Common/Utilities/To.h"
#include "Common/Utilities/Types.h"

// namespaces
namespace niwa {
namespace partition {

/**
 * This method will populate the length data on the partition category object.
 * This is required to be called by any object that wants to update the length
 * data from the age data. For time_steps in a year. There are a few reasons this will be updated between years
 * firstly if age_length typw == data then there will be a different length at age for each year. Secondly if any of the
 * Age length parameters time vary.
 */
void Category::UpdateMeanLengthData() {
  Categories* categories = model_->categories();
  vector<string> time_steps = model_->time_steps();
  unsigned year = model_->current_year();

  if (model_->partition_type() == PartitionType::kAge) {
    AgeLength* age_length = categories->age_length(name_);
    if (!age_length) {
      UpdateMeanWeightData();
    } else {
      // Only do this under three conditions. We are initialising, it has a time varying component, or is of type data.
      if (age_length->is_time_varying() || model_->state() == State::kInitialise || age_length->type() == PARAM_DATA) {
        LOG_FINEST() << "Updating mean length and weight";
        for (unsigned step_iter = 0; step_iter < time_steps.size(); ++step_iter) {
          for (unsigned age = min_age_; age <= max_age_; ++age) {
            mean_length_by_time_step_age_[step_iter][age] = age_length->GetMeanLength(year, step_iter, age);
          }
        }
        // If this has been updated we need to update Mean weight
        UpdateMeanWeightData();
      }
    }
  } else if (model_->partition_type() == PartitionType::kLength) {
    // Don't need to update length cause we are a length structured model, so just update weight
    UpdateMeanWeightData();
  }
}


/**
 * This method will update the weight data with the number of fish and weight
 * per fish for use.
 */

void Category::UpdateMeanWeightData() {
  Categories* categories = model_->categories();
  vector<string> time_steps = model_->time_steps();
  if (model_->partition_type() == PartitionType::kAge) {
    AgeLength* age_length = categories->age_length(name_);
    for (unsigned step_iter = 0; step_iter < time_steps.size(); ++step_iter) {
      for (unsigned age = min_age_; age <= max_age_; ++age)
        mean_weight_by_time_step_age_[step_iter][age] = age_length->mean_weight(step_iter, age);
    }
  } else if (model_->partition_type() == PartitionType::kLength) {
    // Update mean weight for this category
    LengthWeight* length_weight = categories->length_weight(name_);
    // Only do this under two conditions. We are initialising, it has a time varying component
    if (length_weight->is_time_varying() || model_->state() == State::kInitialise) {
      vector<unsigned> length_bins = model_->length_bins();
      for (unsigned step_iter = 0; step_iter < time_steps.size(); ++step_iter) {
        for (unsigned length_bin_index = 0; length_bin_index < length_bins.size(); ++length_bin_index) {
          //Double size = 0;
          //if (!niwa::utilities::To<Double>(length_bins[length_bin_index], size))
          //  LOG_FATAL() << " value (" << length_bins[length_bin_index] << ") could not be converted to a double";

          mean_weight_by_time_step_length_[step_iter][length_bin_index] = length_weight->mean_weight(length_bins[length_bin_index], AgeLength::Distribution::kNone,0.0);
        }
      }
    }
  }
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
void Category::UpdateAgeLengthData(const vector<Double>& length_bins, bool plus_grp, Selectivity* selectivity) {
  LOG_TRACE();

  Categories* categories = model_->categories();
  AgeLength* age_length = categories->age_length(name_);
  if (!age_length)
    LOG_CODE_ERROR() << "if (!age_length) for category " << name_;
  age_length->DoAgeToLengthMatrixConversion(this, length_bins, plus_grp, selectivity);
}


/**
 * This method collapses the Numbers at length by age matrix to numbers at age for a category
 */
void Category::CollapseAgeLengthDataToLength() {
  LOG_TRACE();
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
