/**
 * @file Category.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 18/02/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
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
#include "Utilities/Math.h"
#include <functional>

// namespaces
namespace niwa {
namespace partition {

/**
 * This method populates the length data on the partition category object.
 * This is required to be called by any object that wants to update the length
 * data from the age data. For time_steps in a year.
 *
 * There are a few reasons this will be updated between years:
 * firstly if age_length type == data then there will be a different length at age for each year.
 * secondly if any of the Age length parameters time vary.
 */
void Category::UpdateMeanLengthData() {
  if (mean_length_by_time_step_age_.size() > 0) {
    UpdateMeanWeightData();
    // Categories* categories    = model_->categories();
    // vector<string> time_steps = model_->time_steps();

    // unsigned year     = model_->current_year();
    // unsigned year_ndx = year > model_->start_year() ? year - model_->start_year() : 0;

    // if (model_->partition_type() == PartitionType::kAge) {
    //   AgeLength* age_length = categories->age_length(name_);

    //   // Only do this under three conditions. We are initialising, it has a time varying component (TODO), or is of type data.
    //   if (model_->state() == State::kInitialise || age_length->type() == PARAM_DATA)
    //     LOG_FINE() << "Updating mean length-at-age, then mean weight-at-age for year " << year << " time steps " << time_steps.size();
    //     for (unsigned step_iter = 0; step_iter < time_steps.size(); ++step_iter) {
    //       LOG_FINEST() << "Updating mean length-at-age for year " << year << " time step " << step_iter;
    //       for (unsigned age = min_age_; age <= max_age_; ++age) {
    //         mean_length_by_time_step_age_[year_ndx][step_iter][age] = age_length->GetMeanLength(year, step_iter, age);
    //      }
    //     }

    //     // If this has been updated we need to update Mean weight
    //     UpdateMeanWeightData();
    //   }
    // } else if (model_->partition_type() == PartitionType::kLength) {
    //   // Don't need to update length cause we are a length structured model, so just update weight
    //   UpdateMeanWeightData();
    // }
  }
}

/**
 * This method updates the mean weight values with the number of animals and weight per animal for use.
 */
void Category::UpdateMeanWeightData() {
  Categories* categories    = model_->categories();
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

          mean_weight_by_time_step_length_[step_iter][length_bin_index] = length_weight->mean_weight(length_bins[length_bin_index], Distribution::kNone, 0.0);
        }
      }
    }
  }
}

/**
 * This method populates the age values from the length values. This is required
 * to transfer any changes in the length partition back to the age partition.
 */
void Category::CollapseAgeLengthData() {
  LOG_CODE_ERROR() << "Do not allocate memory with '.push_back'";
  data_.clear();

  unsigned time_step_index = model_->managers().time_step()->current_time_step();

  for (auto age_row : age_length_matrix_[time_step_index]) {
    Double total = 0;
    for (Double length_data : age_row)
      total += length_data;
    data_.push_back(total);
  }
}

/**
 * This method takes the current age population for this category stored
 * in this->data_ and populates this->length_data_ by using the age length
 * proportions generated and stored against the Partition class. The age
 * length proportions are generated during the build phase.
 *
 * @parameter selectivity The selectivity to apply to the age data
 */
void Category::PopulateAgeLengthMatrix(Selectivity* selectivity) {
  LOG_FINE() << "Populating the age-length matrix for category " << name_ << " in year " << model_->current_year();
  // TODO:

  if (selectivity == nullptr)
    LOG_CODE_ERROR() << "selectivity == nullptr";
  if (age_length_ == nullptr)
    LOG_CODE_ERROR() << "In category " << name_ << " there is no age length object to have calculated the age length proportions";
  if (age_length_matrix_.size() == 0)
    LOG_CODE_ERROR() << "No memory has been allocated for the age_length_matrix for category " << name_;

  auto& age_length_proportions = model_->partition().age_length_proportions(name_);
  unsigned year                = model_->current_year() - model_->start_year();
  vector<unsigned> length_bins = model_->length_bins();
  unsigned time_step_index     = model_->managers().time_step()->current_time_step();

  LOG_FINEST() << "Year: " << year << "; time_step: " << time_step_index << "; length_bins: " << length_bins.size();
  LOG_FINEST() << "Years in proportions: " << age_length_proportions.size();
  LOG_FINEST() << "Timesteps in current year: " << age_length_proportions[year].size();

  if (year > age_length_proportions.size())
    LOG_CODE_ERROR() << "year > age_length_proportions.size()";
  if (time_step_index > age_length_proportions[year].size())
    LOG_CODE_ERROR() << "time_step_index > age_length_proportions[year].size()";
  if (age_length_matrix_[time_step_index].size() == 0)
    LOG_CODE_ERROR() << "No memory has been allocated for the age_length_matrix for time step " << time_step_index
      << " in category " << name_;

  vector<vector<Double>>& proportions_for_now = age_length_proportions[year][time_step_index];
  unsigned size = model_->length_plus() == true ? model_->length_bins().size() : model_->length_bins().size() - 1;

  LOG_FINEST() << "Calculating age length data";
  for (unsigned age = min_age_; age <= max_age_; ++age) {
    unsigned i = age - min_age_;
    if (i >= proportions_for_now.size())
      LOG_CODE_ERROR() << "i >= proportions_for_now.size()";
    if (i >= data_.size())
      LOG_CODE_ERROR() << "i >= data_.size()";
    if (i >= age_length_matrix_[time_step_index].size())
      LOG_CODE_ERROR() << "(i >= age_length_matrix_[time_step_index].size())";

    vector<Double>& ages_at_length = proportions_for_now[i];

    for (unsigned bin = 0; bin < size; ++bin) {
      if (bin >= age_length_matrix_[time_step_index][i].size())
        LOG_CODE_ERROR() << "bin (" << bin << ") >= age_length_matrix_[time_step_index][i].size(" << age_length_matrix_[time_step_index][i].size() << ")";
      if (bin >= ages_at_length.size())
        LOG_CODE_ERROR() << "bin >= ages_at_length.size()";

      age_length_matrix_[time_step_index][i][bin] = selectivity->GetAgeResult(age, age_length_) * data_[i] * ages_at_length[bin];
    }
  }

  LOG_FINE() << "Finished populating the age-length matrix for category " << name_ << " in year " << model_->current_year()
    << " and time step " << time_step_index;;
}

/**
 * This method will take the current age population for this category and time step stored
 * in this->data_ and populate @param numbers_by_length_ by using the age length
 * matrix supplied age_length_matrix. This function assumes the all checks are done in the
 * class that passes objects to this method
 *
 * @parameter selectivity The selectivity to apply to the age data
 * @parameter length_bins vector defining length bins
 * @parameter age_length_matrix an empty but has memory allocated matrix for using in the temporary transition
 * @parameter numbers_by_length_ vector that has allocated memory which stores the results
 * @parameter length_plus whether the last bin is a plus group
 */
void Category::CalculateNumbersAtLength(Selectivity* selectivity, const vector<double>& length_bins,
                                        vector<vector<Double>>& age_length_matrix, vector<Double>& numbers_by_length,
                                        const bool& length_plus) {
  // Probably should do some checks and balances, but I want to remove this later on
  unsigned size = length_plus == true ? length_bins.size() : length_bins.size() - 1;
  Double std_dev = 0;

  if (age_length_matrix.size() == 0)
    LOG_CODE_ERROR() << "if (age_length_matrix.size() == 0)";
  if (age_length_matrix.size() != model_->age_spread())
    LOG_CODE_ERROR() << "if (age_length_matrix.size() != model_->age_spread())";
  if (age_length_matrix[0].size() == 0)
    LOG_CODE_ERROR() << "if (age_length_matrix[0].size() == 0)";
  if (age_length_matrix[0].size() != numbers_by_length.size())
    LOG_CODE_ERROR() << "if (age_length_matrix[0].size() != numbers_by_length.size())";

  // Make sure all elements are zero
  std::fill(numbers_by_length.begin(), numbers_by_length.end(), 0.0);

  auto& age_length_proportions = model_->partition().age_length_proportions(name_);
  unsigned year_ndx = model_->current_year() - model_->start_year();
  unsigned time_step_index = model_->managers().time_step()->current_time_step();

  if (year_ndx > age_length_proportions.size())
    LOG_CODE_ERROR() << "year_ndx > age_length_proportions.size()";
  if (time_step_index > age_length_proportions[year_ndx].size())
    LOG_CODE_ERROR() << "time_step_index > age_length_proportions[year].size()";
  vector<vector<Double>>& proportions_for_now = age_length_proportions[year_ndx][time_step_index];

  LOG_FINEST() << "Calculating age length data";
  for (unsigned age = min_age_; age <= max_age_; ++age) {
    unsigned i = age - min_age_;
    std_dev = age_length_->cv(model_->current_year(), time_step_index, age) * mean_length_by_time_step_age_[year_ndx][time_step_index][i];
    if (i >= proportions_for_now.size())
      LOG_CODE_ERROR() << "i >= proportions_for_now.size()";
    if (i >= data_.size())
      LOG_CODE_ERROR() << "i >= data_.size()";
    if (i >= age_length_matrix.size())
      LOG_CODE_ERROR() << "(i >= age_length_matrix.size())";

    // populate age_length matrix with proportions
    age_length_matrix[i] = utilities::math::distribution(length_bins, length_plus, age_length_->distribution(), mean_length_by_time_step_age_[year_ndx][time_step_index][i], std_dev);
    if (age_length_matrix[i].size() != numbers_by_length.size())
      LOG_CODE_ERROR() << "if (age_length_matrix[i].size() != numbers_by_length.size()). Age length dims were "
        << age_length_matrix[i].size() << ", expected " << numbers_by_length.size();
    // Multiply by data_
    std::transform(age_length_matrix[i].begin(), age_length_matrix[i].end(), age_length_matrix[i].begin(), std::bind(std::multiplies<Double>(), std::placeholders::_1, data_[i]));
  }

  // Now collapse down to number at length
  for (unsigned bin = 0; bin < size; ++bin) {
    for (unsigned age = min_age_; age <= max_age_; ++age) {
      unsigned i = age - min_age_;
      numbers_by_length[bin] += age_length_matrix[i][bin];
    }
  }
}

/**
 * This method collapses the Numbers at length by age matrix to numbers at age for a category
 */
void Category::CollapseAgeLengthDataToLength() {
  LOG_TRACE();

  if (age_length_matrix_.size() == 0)
    LOG_CODE_ERROR() << "if (age_length_matrix_.size() == 0)";

  LOG_FINE() << "age_length_matrix_.size(): " << age_length_matrix_.size();             // time steps
  LOG_FINE() << "age_length_matrix_[0].size(): " << age_length_matrix_[0].size();       // ages
  LOG_FINE() << "age_length_matrix_[0][0].size(): " << age_length_matrix_[0][0].size(); // length bins

  length_data_.assign(model_->length_bins().size(), 0.0);

  unsigned time_step_index = model_->managers().time_step()->current_time_step();

  for (unsigned i = 0; i < age_length_matrix_[time_step_index].size(); ++i) {
    for (unsigned j = 0; j < age_length_matrix_[time_step_index][i].size(); ++j) {
      if (j >= length_data_.size())
        LOG_CODE_ERROR() << "j >= length_data_.size()";

      length_data_[j] += age_length_matrix_[time_step_index][i][j];
    }
  }

  for (unsigned i = 0; i < length_data_.size(); ++i)
    LOG_FINEST() << "length_data_[" << i << "]: " << length_data_[i];
}

} /* namespace partitions */
} /* namespace niwa */
