/**
 * @file Increasing.cpp
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
#include "Increasing.h"

#include "../../Model/Model.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Default constructor
 */
Increasing::Increasing(shared_ptr<Model> model) : Selectivity(model) {
  parameters_.Bind<unsigned>(PARAM_L, &low_, "The low value (L)", "");
  parameters_.Bind<unsigned>(PARAM_H, &high_, "The high value (H)", "");
  parameters_.Bind<Double>(PARAM_V, &v_, "The v parameter", "");
  parameters_.Bind<Double>(PARAM_ALPHA, &alpha_, "The maximum value of the selectivity", "", 1.0)->set_lower_bound(0.0, false);

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
void Increasing::DoValidate() {
  if (alpha_ <= 0.0)
    LOG_ERROR_P(PARAM_ALPHA) << ": alpha (" << AS_DOUBLE(alpha_) << ") cannot be less than or equal to 0.0";

  if (high_ <= low_)
    LOG_ERROR_P(PARAM_H) << ": 'h' (" << high_ << ") cannot be less than or equal to 'l' (" << low_ << ")";

  for (unsigned i = 0; i < v_.size(); ++i) {
    if (v_[i] < 0.0 || v_[i] > 1.0) {
      LOG_ERROR_P(PARAM_V) << " 'v' element " << i + 1 << " (" << AS_DOUBLE(v_[i]) << ") must be between 0.0 and 1.0 inclusive";
    }
  }

  if (model_->partition_type() == PartitionType::kAge) {
    if (low_ < model_->min_age() || low_ > model_->max_age())
      LOG_ERROR_P(PARAM_L) << ": 'l' (" << low_ << ") must be between the model min_age (" << model_->min_age() << ") and max_age (" << model_->max_age() << ")";

    if (v_.size() != (high_ - low_ + 1)) {
      LOG_ERROR_P(PARAM_V) << " 'v' has an incorrect number of elements\n"
                           << "Expected: " << (high_ - low_ + 1) << ", parsed " << v_.size();
    }
  } else if (model_->partition_type() == PartitionType::kLength) {
    LOG_ERROR_P(PARAM_LABEL) << " this selectivity type not allowed in length patition models";
    /*
    vector<double> length_bins = model_->length_bin_mid_points();
    if (low_ < length_bins[0] || low_ > length_bins[length_bins.size() - 1])
      LOG_ERROR_P(PARAM_L) << ": 'l' (" << low_ << ") must be between the model min length (" << length_bins[0] << ") and max length (" << length_bins[length_bins.size() - 1]
                           << ")";

    unsigned bins = 0;
    for (unsigned i = 0; i < length_bins.size(); ++i) {
      if (length_bins[i] >= low_ && length_bins[i] <= high_)
        ++bins;
    }
    if (bins != v_.size()) {
      LOG_ERROR_P(PARAM_V) << ": Parameter 'v' has an incorrect number of elements n = low <= length_bins <= high, "
                           << "Expected: " << bins << ", parsed: " << v_.size();
    }
    */
  }
  //lower_length_bin_ = model_->get_length_bin_ndx(low_);
  LOG_FINE() << "lower_length_bin_ " << lower_length_bin_;

}
/**
 * The core function
 */
Double Increasing::get_value(Double value) {
  LOG_CODE_ERROR() << "Increasing::get_value(Double value)";
  /*
  if (model_->partition_type() == PartitionType::kAge) {
  } 
  if (value < low_) {
    return  0.0;
  } else if (value > high_) {
    return *v_.rbegin();
  } else {
    Double result_value = *v_.begin();
    unsigned len_ndx = model_->get_length_bin_ndx(value) - lower_length_bin_;
    LOG_FINE() << "len_ndx " << len_ndx - lower_length_bin_;
    for (unsigned i = lower_length_bin_; i < len_ndx; ++i) {
      LOG_FINE() << "i = " << i;
      if (i > high_ || result_value >= alpha_)
        break;

      result_value += (alpha_ - result_value) * v_[i - len_ndx];
    }
    return result_value;
  }
  */
  return 1.0;
}
/**
 * The core function
 */
Double Increasing::get_value(unsigned value) {
  if (value < low_) {
    return  0.0;
  } else if (value > high_) {
    return *v_.rbegin();
  } else {
    Double result_value = *v_.begin();
    for (unsigned i = low_ + 1; i < value; ++i) {
      if (i > high_ || result_value >= alpha_)
        break;
      result_value += (alpha_ - result_value) * v_[i - low_];
    }
    return result_value;
  }
  return 1.0;
}
} /* namespace selectivities */
} /* namespace niwa */
