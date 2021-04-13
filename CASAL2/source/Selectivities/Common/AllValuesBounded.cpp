/**
 * @file AllValuesBounded.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 14/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "AllValuesBounded.h"

#include <boost/math/distributions/lognormal.hpp>
#include <cmath>

#include "../../AgeLengths/AgeLength.h"
#include "../../Model/Model.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Default constructor
 */
AllValuesBounded::AllValuesBounded(shared_ptr<Model> model)
: Selectivity(model) {

  parameters_.Bind<unsigned>(PARAM_L, &low_, "The low value (L)", "");
  parameters_.Bind<unsigned>(PARAM_H, &high_, "The high value (H)", "");
  parameters_.Bind<Double>(PARAM_V, &v_, "The v parameter", "");

  RegisterAsAddressable(PARAM_V, &v_);
}

/**
 * Validate this selectivity. This will load the
 * values that were passed in from the configuration
 * file and assign them to the local variables.
 *
 * Then do some basic checks on the local
 * variables to ensure they are within the business
 * rules for the model.
 */
void AllValuesBounded::DoValidate() {
  if (model_->partition_type() == PartitionType::kAge) {
    unsigned min_age = model_->min_age();
    unsigned max_age = model_->max_age();

    // Param: L
    if (low_ < min_age) {
      LOG_ERROR_P(PARAM_L) << ": Parameter 'l' is less than the 'min_age'\n"
        << "Model 'min_age' is " << min_age << " and 'l' is " << low_;
    }

    // Param: H
    if (high_ > max_age) {
      LOG_ERROR_P(PARAM_H) << ": Parameter 'h' is greater than the 'max_age'\n"
        << "Model 'max_age' is " << max_age << " and 'h' is " << high_;
    }

    // Param: V
    if (v_.size() != (high_ - low_) + 1) {
      LOG_ERROR_P(PARAM_V) << ": Parameter 'v' has an incorrect number of elements n = h - l\n"
        << "Expected: " << (high_ - low_) + 1 << ", parsed: " << v_.size();
    }

  } else if (model_->partition_type() == PartitionType::kLength) {
    vector<Double> length_bins = model_->length_bins();
    unsigned bins = 0;
    for (unsigned i = 0; i < length_bins.size(); ++i) {
      if (length_bins[i] >= low_ && length_bins[i] <= high_)
        ++bins;
    }
    if (bins != v_.size()) {
      LOG_ERROR_P(PARAM_V) << ": Parameter 'v' has an incorrect number of elements n = low <= length_bins <= high, "
        << "Expected: " << bins << ", parsed: " << v_.size();
    }


  }

  if (low_ >= high_) {
    LOG_ERROR_P(PARAM_L) << ": Parameter 'l' is greater than or equal to parameter 'h'\n"
      << "'l' = " << low_ << " and 'h' = " << high_;
  }

}

/**
 * Reset this selectivity so it is ready for the next execution
 * phase in the model.
 *
 * This method will rebuild the cache of selectivity values
 * for each age or length in the model.
 */
void AllValuesBounded::RebuildCache() {
  /**
   * Resulting age map should look like
   * While Age < Low :: Value = 0.0
   * While Age > Low && Age < High :: Value = v_
   * While age > High :: Value = Last element if v_
   */
  if (model_->partition_type() == PartitionType::kAge) {
    unsigned min_age = model_->min_age();
    unsigned max_age = model_->max_age();
    unsigned age = min_age;
    for (; age < low_; ++age)
      values_[age - age_index_] = 0.0;
    for (unsigned i = 0; i < v_.size(); ++i, ++age) {
      if (v_[i] < 0.0)
        LOG_FATAL_P(PARAM_V) << "v cannot have values less than 0.0. value = " << v_[i] << " for age = " << age;
      values_[age - age_index_] = v_[i];
    }
    for (; age <= max_age; ++age)
      values_[age - age_index_] = *v_.rbegin();

  } else if (model_->partition_type() == PartitionType::kLength) {
    vector<Double> length_bins = model_->length_bins();
    unsigned v_index = 0;
    for (unsigned length_bin_index = 0; length_bin_index < length_bins.size(); ++length_bin_index)
      if (length_bins[length_bin_index] < low_)
        length_values_[length_bin_index] = 0.0;
      else if (length_bins[length_bin_index] > high_) {
        length_values_[length_bin_index] = *v_.rbegin();
      } else {
        length_values_[length_bin_index] = v_[v_index];
        ++v_index;
      }
  }
}

/**
 * GetLengthBasedResult function
 *
 * @param age
 * @param age_length AgeLength pointer
 * @param year
 * @param time_step_index
 * @return 0.0 - error
 */

Double AllValuesBounded::GetLengthBasedResult(unsigned age, AgeLength* age_length, unsigned year, int time_step_index) {
  LOG_ERROR_P(PARAM_LENGTH_BASED) << ": This selectivity type has not been implemented for age length based selectivities ";
  return 0.0;
}

} /* namespace selectivities */
} /* namespace niwa */
