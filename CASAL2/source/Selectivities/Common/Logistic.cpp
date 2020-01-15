/**
 * @file Logistic.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include <boost/math/distributions/lognormal.hpp>
#include <Selectivities/Common/Logistic.h>
#include <cmath>

#include "AgeLengths/AgeLength.h"
#include "Model/Model.h"
#include "TimeSteps/Manager.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Explicit Constructor
 */
Logistic::Logistic(Model* model)
: Selectivity(model) {

  parameters_.Bind<Double>(PARAM_A50, &a50_, "A50", "");
  parameters_.Bind<Double>(PARAM_ATO95, &ato95_, "Ato95", "")->set_lower_bound(0.0, false);
  parameters_.Bind<Double>(PARAM_ALPHA, &alpha_, "Alpha", "", 1.0)->set_lower_bound(0.0, false);

  RegisterAsAddressable(PARAM_A50, &a50_);
  RegisterAsAddressable(PARAM_ATO95, &ato95_);
  RegisterAsAddressable(PARAM_ALPHA, &alpha_);
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
void Logistic::DoValidate() {
  if (alpha_ <= 0.0)
    LOG_ERROR_P(PARAM_ALPHA) << ": alpha (" << AS_DOUBLE(alpha_) << ") cannot be less than or equal to 0.0";
  if (ato95_ <= 0.0)
    LOG_ERROR_P(PARAM_ATO95) << ": ato95 (" << AS_DOUBLE(ato95_) << ") cannot be less than or equal to 0.0";
}

/**
 * Reset this selectivity so it's ready for the next execution
 * phase in the model.
 *
 * This method will rebuild the cache of selectivity values
 * for each age in the model.
 */
void Logistic::RebuildCache() {
  if (model_->partition_type() == PartitionType::kAge) {
    Double threshold = 0.0;

    for (unsigned age = model_->min_age(); age <= model_->max_age(); ++age) {
      threshold = (a50_ - (Double)age) / ato95_;

      if (threshold > 5.0)
        values_[age - age_index_] = 0.0;
      else if (threshold < -5.0)
        values_[age - age_index_] = alpha_;
      else
        values_[age - age_index_] = alpha_ / (1.0 + pow(19.0, threshold));
    }
  } else if (model_->partition_type() == PartitionType::kLength) {
    Double threshold = 0.0;
    vector<unsigned> length_bins = model_->length_bins();

    for (unsigned length_bin_index = 0; length_bin_index < length_bins.size(); ++length_bin_index) {
      Double temp = (Double)length_bins[length_bin_index];
      threshold = (Double)(a50_ - temp) / ato95_;
      if (threshold > 5.0)
        length_values_[length_bin_index] = 0.0;
      else if (threshold < -5.0)
        length_values_[length_bin_index] = alpha_;
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
 * @return Double selectivity for an age based on age length distribution_label
 */

Double Logistic::GetLengthBasedResult(unsigned age, AgeLength* age_length, unsigned year, int time_step_index) {
  unsigned yearx = year == 0 ? model_->current_year() : year;
  unsigned time_step = time_step_index == -1 ? model_->managers().time_step()->current_time_step() : (unsigned)time_step_index;
  Double cv = age_length->cv(yearx, time_step, age);
  Double mean = age_length->mean_length(time_step, age);
  string dist = age_length->distribution_label();

  if (dist == PARAM_NONE || n_quant_ <= 1) {
    // no distribution_label just use the mu from age_length
    Double threshold = (a50_ - (Double) mean) / ato95_;

    if (threshold > 5.0)
      return 0.0;
    else if (threshold < -5.0)
      return alpha_;
    else
      return alpha_ / (1.0 + pow(19.0, threshold));

  } else if (dist == PARAM_NORMAL) {

    Double sigma = cv * mean;
    Double size = 0.0;
    Double total = 0.0;

    for (unsigned j = 0; j < n_quant_; ++j) {
      size = mean + sigma * quantiles_at_[j];

      Double threshold = (a50_ - size) / ato95_;

      if (threshold > 5.0)
        total += 0.0;
      else if (threshold < -5.0)
        total += alpha_;
      else
        total += alpha_ / (1.0 + pow(19.0, threshold));
    }
    return total / n_quant_;

  } else if (dist == PARAM_LOGNORMAL) {
    // convert paramters to log space
    Double sigma = sqrt(log(1 + cv * cv));
    Double mu = log(mean) - sigma * sigma * 0.5;
    Double size = 0.0;
    Double total = 0.0;
    boost::math::lognormal dist{AS_VALUE(mu), AS_VALUE(sigma)};

    for (unsigned j = 0; j < n_quant_; ++j) {
      size = mu + sigma * quantile(dist, AS_VALUE(quantiles_[j]));

      Double threshold = (a50_ - (Double) size) / ato95_;

      if (threshold > 5.0)
        total += 0.0;
      else if (threshold < -5.0)
        total += alpha_;
      else
        total += alpha_ / (1.0 + pow(19.0, threshold));
    }
    return total / n_quant_;
  }
  LOG_CODE_ERROR() << "dist is invalid " << dist;
  return 0;
}

} /* namespace selectivities */
} /* namespace niwa */
