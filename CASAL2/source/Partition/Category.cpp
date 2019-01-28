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

#include "AgeLengths/AgeLength.h"
#include "LengthWeights/LengthWeight.h"
#include "Categories/Categories.h"
#include "Model/Model.h"
#include "Partition/Partition.h"
#include "TimeSteps/Manager.h"
#include "Utilities/To.h"
#include "Utilities/Types.h"

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
  if (mean_length_by_time_step_age_.size() > 0)
    UpdateMeanWeightData();
//  LOG_WARNING() << "This method no longer exists";
//  Categories* categories = model_->categories();
//  vector<string> time_steps = model_->time_steps();
//  unsigned year = model_->current_year();
//
//  if (model_->partition_type() == PartitionType::kAge) {
//    AgeLength* age_length = categories->age_length(name_);
//
//    // Only do this under three conditions. We are initialising, it has a time varying component, or is of type data.
//    if (age_length->is_time_varying() || model_->state() == State::kInitialise || age_length->type() == PARAM_DATA) {
//      LOG_FINEST() << "Updating mean length and weight";
//      for (unsigned step_iter = 0; step_iter < time_steps.size(); ++step_iter) {
//        for (unsigned age = min_age_; age <= max_age_; ++age) {
//          mean_length_by_time_step_age_[step_iter][age] = age_length->GetMeanLength(year, step_iter, age);
//        }
//      }
//      // If this has been updated we need to update Mean weight
//      UpdateMeanWeightData();
//    }
//  } else if (model_->partition_type() == PartitionType::kLength) {
//    // Don't need to update length cause we are a length structured model, so just update weight
//    UpdateMeanWeightData();
//  }
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
    if (age_length == nullptr) {
      for (unsigned step_iter = 0; step_iter < time_steps.size(); ++step_iter) {
        for (unsigned age = min_age_; age <= max_age_; ++age)
          mean_weight_by_time_step_age_[step_iter][age] = 1.0;
      }
    } else {
      for (unsigned step_iter = 0; step_iter < time_steps.size(); ++step_iter) {
        for (unsigned age = min_age_; age <= max_age_; ++age)
          mean_weight_by_time_step_age_[step_iter][age] = age_length->mean_weight(step_iter, age);
      }
    }

  } else if (model_->partition_type() == PartitionType::kLength) {
    // Update mean weight for this category
    LengthWeight* length_weight = categories->length_weight(name_);
    // Only do this under two conditions. We are initialising, it has a time varying component
    if (model_->state() == State::kInitialise) {
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
  LOG_CODE_ERROR() << "This is hideously slow, do not allocate memory with the .push_back";
  data_.clear();

  for (auto age_row : age_length_matrix_) {
    Double total = 0;
    for (Double length_data : age_row)
      total += length_data;
    data_.push_back(total);
  }
}

/**
 * This method will take the current age population for this category stored
 * in this->data_ and populate this->length_data_ by using the age length
 * proportions generated and stored against the Partition class. The age
 * length proportions are generated during the build phase.
 *
 * @parameter selectivity The selectivity to apply to the age data
 */
void Category::PopulateAgeLengthMatrix(Selectivity* selectivity) {
  LOG_FINEST() << "About to populate the length data for category " << name_ << " in year " << model_->current_year();

  if (selectivity == nullptr)
    LOG_CODE_ERROR() << "selectivity == nullptr";
  if (age_length_ == nullptr)
    LOG_CODE_ERROR() << "In category " << name_ << " there is no age length object to have calculated the age length proportions";
  if (age_length_matrix_.size() == 0)
    LOG_CODE_ERROR() << "No memory has been allocated for the age_length_matrix for category " << name_;

  auto& age_length_proportions = model_->partition().age_length_proportions(name_);
  unsigned year = model_->current_year() - model_->start_year();
  vector<unsigned> length_bins = model_->length_bins();
  unsigned time_step_index = model_->managers().time_step()->current_time_step();

  LOG_FINEST() << "Year: " << year << "; time_step: " << time_step_index << "; length_bins: " << length_bins.size();
  LOG_FINEST() << "Years in proportions: " << age_length_proportions.size();
  LOG_FINEST() << "Timesteps in current year: " << age_length_proportions[year].size();

  if (year > age_length_proportions.size())
    LOG_CODE_ERROR() << "year > age_length_proportions.size()";
  if (time_step_index > age_length_proportions[year].size())
    LOG_CODE_ERROR() << "time_step_index > age_length_proportions[year].size()";
  vector<vector<Double>>& proportions_for_now = age_length_proportions[year][time_step_index];

  unsigned size = model_->length_plus() == true ? model_->length_bins().size() : model_->length_bins().size() - 1;
  LOG_FINEST() << "Calculating age length data";
  for (unsigned age = min_age_; age <= max_age_; ++age) {
    unsigned i = age - min_age_;
    if (i >= proportions_for_now.size())
      LOG_CODE_ERROR() << "i >= proportions_for_now.size()";
    if (i >= data_.size())
      LOG_CODE_ERROR() << "i >= data_.size()";
    if (i >= age_length_matrix_.size())
      LOG_CODE_ERROR() << "(i >= age_length_matrix_.size())";

    vector<Double>& ages_at_length = proportions_for_now[i];

    for (unsigned bin = 0; bin < size; ++bin) {
      if (bin >= age_length_matrix_[i].size())
        LOG_CODE_ERROR() << "bin (" << bin << ") >= age_length_matrix_[i].size(" << age_length_matrix_[i].size() << ")";
      if (bin >= ages_at_length.size())
        LOG_CODE_ERROR() << "bin >= ages_at_length.size()";

      age_length_matrix_[i][bin] = selectivity->GetAgeResult(age, age_length_) * data_[i] * ages_at_length[bin];
    }
  }

  LOG_FINEST() << "Finished populating the length data for category " << name_ << " in year " << model_->current_year();
}

/**
 * This method collapses the Numbers at length by age matrix to numbers at age for a category
 */
void Category::CollapseAgeLengthDataToLength() {
  LOG_TRACE();

  if (age_length_matrix_.size() == 0)
    LOG_CODE_ERROR() << "if (age_length_matrix_.size() == 0)";

  LOG_FINE() << "age_length_matrix_.size(): " << age_length_matrix_.size();
  LOG_FINE() << "age_length_matrix_[0].size(): " << age_length_matrix_[0].size();
  length_data_.assign(model_->length_bins().size(), 0.0);
  for (unsigned i = 0; i < age_length_matrix_.size(); ++i) {
    for (unsigned j = 0; j < age_length_matrix_[i].size(); ++j) {
      if (j >= length_data_.size())
        LOG_CODE_ERROR() << "j >= length_data_.size()";

      length_data_[j] += age_length_matrix_[i][j];
    }
  }

  for (unsigned i = 0; i < length_data_.size(); ++i)
    LOG_FINEST() << "length_data_[" << i << "]: " << length_data_[i];
}


} /* namespace partitions */
} /* namespace niwa */
