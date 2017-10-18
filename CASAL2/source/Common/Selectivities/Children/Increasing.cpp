/**
 * @file Increasing.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 14/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Increasing.h"

#include <boost/math/distributions/lognormal.hpp>
#include <cmath>

#include "Age/AgeLengths/AgeLength.h"
#include "Common/Model/Model.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Explicit constructor
 */
Increasing::Increasing(Model* model)
: Selectivity(model) {

  parameters_.Bind<unsigned>(PARAM_L, &low_, "Low", "");
  parameters_.Bind<unsigned>(PARAM_H, &high_, "High", "");
  parameters_.Bind<Double>(PARAM_V, &v_, "V", "");
  parameters_.Bind<Double>(PARAM_ALPHA, &alpha_, "Alpha", "", 1.0);

  RegisterAsAddressable(PARAM_V, &v_);
}

/**
 * Validate this selectivity. This will load the
 * values that were passed in from the configuration
 * file and assign them to the local variables.
 *
 * We'll then do some basic checks on the local
 * variables to ensure they are within the business
 * rules for the model.
 */
void Increasing::DoValidate() {

  if (alpha_ <= 0.0)
    LOG_ERROR_P(PARAM_ALPHA) << ": alpha (" << AS_DOUBLE(alpha_) << ") cannot be less than or equal to 0.0";

  if (high_ <= low_)
    LOG_ERROR_P(PARAM_H) << ": 'h' (" << high_ << ") cannot be less than or the same as 'l' (" << low_ << ")";

  for (unsigned i = 0; i < v_.size(); ++i) {
    if (v_[i] < 0.0 || v_[i] > 1.0) {
      LOG_ERROR_P(PARAM_V) << " 'v' element " << i + 1 << " (" << AS_DOUBLE(v_[i]) << ") is not between 0.0 and 1.0";
    }
  }

  if (model_->partition_type() == PartitionType::kAge) {
    if (low_ < model_->min_age() || low_ > model_->max_age())
      LOG_ERROR_P(PARAM_L) << ": 'l' (" << low_ << ") must be between the model min_age (" << model_->min_age() << ") and max_age (" << model_->max_age() << ")";

    if (v_.size() != (high_ - low_ + 1)) {
      LOG_ERROR_P(PARAM_V) << " 'v' has incorrect amount of elements\n"
          << "Expected: " << (high_ - low_ + 1) << " but got " << v_.size();
    }
  }   else if (model_->partition_type() == PartitionType::kLength) {
    vector<unsigned> length_bins = model_->length_bins();
    if (low_ < length_bins[0] || low_ > length_bins[length_bins.size()-1])
      LOG_ERROR_P(PARAM_L) << ": 'l' (" << low_ << ") must be between the model min length (" << length_bins[0] << ") and max length (" << length_bins[length_bins.size()-1] << ")";
    unsigned bins = 0;
    for (unsigned i = 0; i < length_bins.size(); ++i) {
      if (length_bins[i] >= low_ && length_bins[i] <= high_)
        ++bins;
    }
    if (bins != v_.size()) {
      LOG_ERROR_P(PARAM_V) << ": Parameter 'v' does not have the right amount of elements n = low <= length_bins <= high, "
          << "Expected " << bins << " but got " << v_.size();
    }
  }




}

/**
 * Reset this selectivity so it's ready for the next execution
 * phase in the model.
 *
 * This method will rebuild the cache of selectivity values
 * for each age in the model.
 */
void Increasing::Reset() {
  if (model_->partition_type() == PartitionType::kAge) {
    for (unsigned age = model_->min_age(); age <= model_->max_age(); ++age) {

      if (age < low_) {
        values_[age] = 0.0;

      } else if (age > high_) {
        values_[age] = *v_.rbegin();

      } else {
        Double value = *v_.begin();
        for (unsigned i = low_ + 1; i < age; ++i) {
          if (i > high_ || value >= alpha_)
            break;
          value += (alpha_ - value) * v_[i - low_];
        }

        values_[age] = value;
      }
    }
  } else if (model_->partition_type() == PartitionType::kLength) {
    vector<unsigned> length_bins = model_->length_bins();
    unsigned mark = 0;
    unsigned start_element = 0;
    while (mark == 0) {
      ++start_element;
      if (length_bins[start_element] >= low_) {
        ++mark;
      }
    }
    cout << "Start element is " << start_element << endl;
    for (unsigned length_bin_index = 0; length_bin_index < length_bins.size(); ++length_bin_index) {
      Double temp = (Double)length_bins[length_bin_index];
      if (temp < low_) {
        length_values_[length_bin_index] = 0.0;

      } else if (temp > high_) {
        length_values_[length_bin_index] = *v_.rbegin();

      } else {
        Double value = *v_.begin();
        for (unsigned i = start_element + 1; i < length_bin_index; ++i) {
          if (length_bins[i] > high_ || value >= alpha_)
            break;
          value += (alpha_ - value) * v_[i - start_element];
        }

        length_values_[length_bin_index] = value;
      }
      cout << "Element " << length_bin_index << " is " << length_values_[length_bin_index] << endl;
    }

  }
}

/**
 * GetLengthBasedResult function
 *
 * @param age
 * @param age_length AgeLength pointer
 * @return Double selectivity for an age based on age length distribution
 */

Double Increasing::GetLengthBasedResult(unsigned age, AgeLength* age_length) {
  LOG_ERROR_P(PARAM_LENGTH_BASED) << ": This selectivity type has not been implemented for length based selectivities ";
  return 0.0;
}

} /* namespace selectivities */
} /* namespace niwa */
