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
AllValuesBounded::AllValuesBounded(shared_ptr<Model> model) : Selectivity(model) {
  parameters_.Bind<unsigned>(PARAM_L, &low_, "The low value (L)", "");
  parameters_.Bind<unsigned>(PARAM_H, &high_, "The high value (H)", "");
  parameters_.Bind<Double>(PARAM_V, &v_, "The v parameter", "");

  RegisterAsAddressable(PARAM_V, &v_);
  allowed_length_based_in_age_based_model_ = false;

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
    vector<double> length_bins = model_->length_bin_mid_points();
    unsigned length_low_ndx = model_->get_length_bin_ndx(low_);
    unsigned length_high_ndx = model_->get_length_bin_ndx(high_);
    //cerr << "low length = " << length_low_ndx << " high length ndx = " << length_high_ndx << " diff = " << (length_low_ndx - length_high_ndx) + 1 << " v_size = " << v_.size();
    if (v_.size() != (length_high_ndx - length_low_ndx) + 1) {
      LOG_ERROR_P(PARAM_V) << ": Parameter 'v' has an incorrect number of elements n = low <= length_bins <= high, "
                           << "Expected: " << (length_high_ndx - length_low_ndx) + 1 << ", parsed: " << v_.size();
    }
  }

  if (low_ >= high_) {
    LOG_ERROR_P(PARAM_L) << ": Parameter 'l' is greater than or equal to parameter 'h'\n"
                         << "'l' = " << low_ << " and 'h' = " << high_;
  }

  lower_length_bin_ = model_->get_length_bin_ndx(low_);
  LOG_FINE() << "lower_length_bin_ = " << lower_length_bin_;
}

/**
 * The core function
 */
Double AllValuesBounded::get_value(Double value) {
  if( value < low_) {
    return 0.0;
  } else if(value > high_) {
    return *v_.rbegin();
  } else {
    if (model_->partition_type() == PartitionType::kAge) {
      LOG_CODE_ERROR() << "model_->partition_type() == PartitionType::kAge";
    } else {
      // Length based a little more tricky
      unsigned len_ndx = model_->get_length_bin_ndx(value);
      LOG_FINE() << "len " << len_ndx;
      return v_[len_ndx - lower_length_bin_];
    }
  }
  LOG_CODE_ERROR() << "AllValuesBounded::get_value(Double value) value = "  << value;
  return 1.0;
}

/**
 * The core function
 */
Double AllValuesBounded::get_value(unsigned value) {
  if( value < low_) {
    return 0.0;
  } else if(value > high_) {
    return *v_.rbegin();
  } else {
    if (model_->partition_type() == PartitionType::kAge) {
      return v_[value - low_];
    } else {
      // Length based a little more tricky
      LOG_CODE_ERROR() << "model_->partition_type() == PartitionType::kLength";
    }
  }
  LOG_CODE_ERROR() << "AllValuesBounded::get_value(unsigned value) value = "  << value;
  return 1.0;
}
} /* namespace selectivities */
} /* namespace niwa */
