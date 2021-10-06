/**
 * @file LogisticProducing.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "LogisticProducing.h"

#include <boost/math/distributions/lognormal.hpp>
#include <cmath>

#include "../../AgeLengths/AgeLength.h"
#include "../../Model/Model.h"

// namespaces
namespace niwa {
namespace selectivities {

/**
 * Default constructor
 */
LogisticProducing::LogisticProducing(shared_ptr<Model> model) : Selectivity(model) {
  parameters_.Bind<unsigned>(PARAM_L, &low_, "The low value (L)", "");
  parameters_.Bind<unsigned>(PARAM_H, &high_, "The high value (H)", "");
  parameters_.Bind<Double>(PARAM_A50, &a50_, "the a50 parameter", "");
  parameters_.Bind<Double>(PARAM_ATO95, &ato95_, "The ato95 parameter", "")->set_lower_bound(0.0, false);
  parameters_.Bind<Double>(PARAM_ALPHA, &alpha_, "The maximum value of the selectivity", "", 1.0)->set_lower_bound(0.0, false);

  RegisterAsAddressable(PARAM_A50, &a50_);
  RegisterAsAddressable(PARAM_ATO95, &ato95_);
  RegisterAsAddressable(PARAM_ALPHA, &alpha_);
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
void LogisticProducing::DoValidate() {
  if (low_ > a50_ || high_ < a50_ || low_ >= high_) {
    LOG_ERROR_P(PARAM_A50) << "low (l) cannot be greater than a50 | high (h) cannot be less than a50 | low cannot be greater than high";
  }

  if (alpha_ <= 0.0)
    LOG_ERROR_P(PARAM_ALPHA) << ": alpha (" << AS_DOUBLE(alpha_) << ") cannot be less than or equal to 0.0";
  if (ato95_ <= 0.0)
    LOG_ERROR_P(PARAM_ATO95) << ": ato95 (" << AS_DOUBLE(ato95_) << ") cannot be less than or equal to 0.0";
}

/**
 * Reset this selectivity so it is ready for the next execution
 * phase in the model.
 *
 * This method will rebuild the cache of selectivity values
 * for each age or length in the model.
 */
void LogisticProducing::RebuildCache() {
  if (model_->partition_type() == PartitionType::kAge) {
    Double temp = 0.0;
    for (unsigned age = model_->min_age(); age <= model_->max_age(); ++age) {
      temp = age;
      if (age < low_)
        values_[age - age_index_] = 0.0;
      else if (age >= high_)
        values_[age - age_index_] = alpha_;
      else if (age == low_)
        values_[age - age_index_] = alpha_ / (1.0 + pow(19.0, (a50_ - temp) / ato95_));
      else {
        Double lambda2 = 1.0 / (1.0 + pow(19.0, (a50_ - (temp - 1)) / ato95_));
        if (lambda2 > 0.99999) {
          values_[age - age_index_] = alpha_;
        } else {
          Double lambda1            = 1.0 / (1.0 + pow(19.0, (a50_ - temp) / ato95_));
          values_[age - age_index_] = (lambda1 - lambda2) / (1.0 - lambda2) * alpha_;
          LOG_FINEST() << "age = " << age << " lambda1 = " << lambda1 << " lambda2 = " << lambda2 << " value = " << values_[age - age_index_];
        }
      }
    }
  } else if (model_->partition_type() == PartitionType::kLength) {
    vector<double> length_bins = model_->length_bins();
    Double         temp        = 0.0;

    for (unsigned length_bin_index = 0; length_bin_index < length_bins.size(); ++length_bin_index) {
      temp = length_bins[length_bin_index];
      if (temp < low_)
        length_values_[length_bin_index] = 0.0;
      else if (temp >= high_)
        length_values_[length_bin_index] = alpha_;
      else if (temp == low_)
        length_values_[length_bin_index] = alpha_ / (1.0 + pow(19.0, (a50_ - temp) / ato95_));
      else {
        Double lambda2 = 1.0 / (1.0 + pow(19.0, (a50_ - (temp - 1)) / ato95_));
        if (lambda2 > 0.99999) {
          length_values_[length_bin_index] = alpha_;
        } else {
          Double lambda1                   = 1.0 / (1.0 + pow(19.0, (a50_ - temp) / ato95_));
          length_values_[length_bin_index] = (lambda1 - lambda2) / (1.0 - lambda2) * alpha_;
          LOG_FINEST() << "length = " << temp << " lambda1 = " << lambda1 << " lambda2 = " << lambda2 << " value = " << length_values_[length_bin_index];
        }
      }
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
 * @return Double selectivity for an age based on age length distribution
 */

Double LogisticProducing::GetLengthBasedResult(unsigned age, AgeLength* age_length, unsigned year, int time_step_index) {
  LOG_ERROR_P(PARAM_LENGTH_BASED) << ": This selectivity type has not been implemented for length-based selectivities in an age-based model";
  return 0.0;
}

} /* namespace selectivities */
} /* namespace niwa */
