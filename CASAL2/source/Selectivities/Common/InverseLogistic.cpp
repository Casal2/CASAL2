/**
 * @file InverseLogistic.cpp
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
#include "InverseLogistic.h"

#include <boost/math/distributions/lognormal.hpp>
#include <cmath>

#include "../../AgeLengths/AgeLength.h"
#include "../../Model/Model.h"
#include "../../TimeSteps/Manager.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Default constructor
 */
InverseLogistic::InverseLogistic(shared_ptr<Model> model) : Selectivity(model) {
  parameters_.Bind<Double>(PARAM_A50, &a50_, "a50", "");
  parameters_.Bind<Double>(PARAM_ATO95, &ato95_, "ato95", "")->set_lower_bound(0.0, false);
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
void InverseLogistic::DoValidate() {
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
void InverseLogistic::RebuildCache() {
  if (model_->partition_type() == PartitionType::kAge) {
    Double threshold = 0.0;

    for (unsigned age = model_->min_age(); age <= model_->max_age(); ++age) {
      threshold = (a50_ - age) / ato95_;

      if (threshold > 5.0)
        values_[age - age_index_] = alpha_;
      else if (threshold < -5.0)
        values_[age - age_index_] = 0.0;
      else
        values_[age - age_index_] = alpha_ - (alpha_ / (1.0 + pow(19.0, threshold)));
    }
  } else if (model_->partition_type() == PartitionType::kLength) {
    vector<double> length_bins = model_->length_bins();
    Double         threshold   = 0.0;

    for (unsigned length_bin_index = 0; length_bin_index < length_bins.size(); ++length_bin_index) {
      threshold = (a50_ - length_bins[length_bin_index]) / ato95_;
      if (threshold > 5.0)
        length_values_[length_bin_index] = alpha_;
      else if (threshold < -5.0)
        length_values_[length_bin_index] = 0.0;
      else
        length_values_[length_bin_index] = alpha_ - (alpha_ / (1.0 + pow(19.0, threshold)));
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
 * @return Double selectivity for an age based on age length distribution_label
 */

Double InverseLogistic::GetLengthBasedResult(unsigned age, AgeLength* age_length, unsigned year, int time_step_index) {
  unsigned yearx     = year == 0 ? model_->current_year() : year;
  unsigned time_step = model_->managers()->time_step()->current_time_step();

  Double cv   = age_length->cv(yearx, time_step, age);
  Double mean = age_length->mean_length(time_step, age);
  string dist = age_length->distribution_label();

  Double threshold = 0.0;

  if (dist == PARAM_NONE || n_quant_ <= 1) {
    // no distribution_label just use the mu from age_length
    threshold = (a50_ - mean) / ato95_;

    if (threshold > 5.0)
      return alpha_;
    else if (threshold < -5.0)
      return 0.0;
    else
      return alpha_ - (alpha_ / (1.0 + pow(19.0, threshold)));

  } else if (dist == PARAM_NORMAL) {
    Double sigma = cv * mean;
    Double size  = 0.0;
    Double total = 0.0;

    for (unsigned j = 0; j < n_quant_; ++j) {
      size = mean + sigma * quantiles_at_[j];

      threshold = (a50_ - size) / ato95_;

      if (threshold > 5.0)
        total += alpha_;
      else if (threshold < -5.0)
        total += 0.0;
      else
        total += alpha_ - (alpha_ / (1.0 + pow(19.0, threshold)));
    }

    return total / n_quant_;

  } else if (dist == PARAM_LOGNORMAL) {
    // convert paramters to log space
    Double                 sigma = sqrt(log(1 + cv * cv));
    Double                 mu    = log(mean) - sigma * sigma * 0.5;
    Double                 size  = 0.0;
    Double                 total = 0.0;
    boost::math::lognormal dist{AS_DOUBLE(mu), AS_DOUBLE(sigma)};

    for (unsigned j = 0; j < n_quant_; ++j) {
      size = quantile(dist, AS_DOUBLE(quantiles_[j]));

      threshold = (a50_ - size) / ato95_;

      if (threshold > 5.0)
        total += alpha_;
      else if (threshold < -5.0)
        total += 0.0;
      else
        total += alpha_ - (alpha_ / (1.0 + pow(19.0, threshold)));
    }
    return total / n_quant_;
  }
  LOG_CODE_ERROR() << "The specified distribution is not a valid distribution: " << dist;
  return 0;
}

} /* namespace selectivities */
} /* namespace niwa */
