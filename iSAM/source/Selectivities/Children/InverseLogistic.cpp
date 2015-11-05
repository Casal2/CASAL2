/**
 * @file InverseLogistic.cpp
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
#include "InverseLogistic.h"

#include <boost/math/distributions/lognormal.hpp>
#include <cmath>

#include "AgeLengths/AgeLength.h"
#include "Model/Model.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Explicit constructor
 */
InverseLogistic::InverseLogistic(Model* model)
  : Selectivity(model) {

  parameters_.Bind<Double>(PARAM_A50, &a50_, "A50", "");
  parameters_.Bind<Double>(PARAM_ATO95, &aTo95_, "aTo95", "");
  parameters_.Bind<Double>(PARAM_ALPHA, &alpha_, "Alpha", "", 1.0);

  RegisterAsEstimable(PARAM_A50, &a50_);
  RegisterAsEstimable(PARAM_ATO95, &aTo95_);
  RegisterAsEstimable(PARAM_ALPHA, &alpha_);
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
void InverseLogistic::DoValidate() {
  if (alpha_ <= 0.0)
    LOG_ERROR_P(PARAM_ALPHA) << ": alpha (" << AS_DOUBLE(alpha_) << ") cannot be less than or equal to 0.0";
  if (aTo95_ <= 0.0)
    LOG_ERROR_P(PARAM_ATO95) << ": ato95 (" << AS_DOUBLE(aTo95_) << ") cannot be less than or equal to 0.0";
}

/**
 * Reset this selectivity so it's ready for the next execution
 * phase in the model.
 *
 * This method will rebuild the cache of selectivity values
 * for each age in the model.
 */
void InverseLogistic::Reset() {
  Double threshold = 0.0;

  for (unsigned age = model_->min_age(); age <= model_->max_age(); ++age) {
	  Double temp = (Double)age;
    threshold = (Double)(a50_ - temp) / aTo95_;

    if (threshold > 5.0)
      values_[age] = alpha_;
    else if (threshold < -5.0)
      values_[age] = 0.0;
    else
      values_[age] = alpha_ - (alpha_ / (1.0 + pow(19.0, threshold)));
  }
}
/**
 * GetLengthBasedResult function
 *
 * @param age
 * @param age_length AgeLength pointer
 * @return Double selectivity for an age based on age length distribution
 */

Double InverseLogistic::GetLengthBasedResult(unsigned age, AgeLength* age_length) {
  Double cv = age_length->cv(age);
  unsigned year = model_->current_year();
  Double mean = age_length->mean_length(year, age);
  string dist = age_length->distribution();

  if (dist == PARAM_NONE || n_quant_ <= 1) {
    // no distribution just use the mu from age_length
    Double threshold = (a50_ - (Double) mean) / aTo95_;

    if (threshold > 5.0)
      return alpha_;
    else if (threshold < -5.0)
      return 0.0;
    else
      return alpha_ - (alpha_ / (1.0 + pow(19.0, threshold)));

  } else if (dist == PARAM_NORMAL) {

    Double sigma = cv * mean;
    Double size = 0.0;
    Double total = 0.0;

    for (unsigned j = 0; j < n_quant_; ++j) {
      size = mean + sigma * quantiles_at_[j];

      Double threshold = (a50_ - size) / aTo95_;

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
    Double sigma = sqrt(log(1 + cv * cv));
    Double mu = log(mean) - sigma * sigma * 0.5;
    Double size = 0.0;
    Double total = 0.0;
    boost::math::lognormal dist{AS_DOUBLE(mu), AS_DOUBLE(sigma)};

    for (unsigned j = 0; j < n_quant_; ++j) {
      size = mu + sigma * quantile(dist, AS_DOUBLE(quantiles_[j]));

      Double threshold = (a50_ - (Double) size) / aTo95_;

      if (threshold > 5.0)
        total += alpha_;
      else if (threshold < -5.0)
        total += 0.0;
      else
        total += alpha_ - (alpha_ / (1.0 + pow(19.0, threshold)));
    }
    return total / n_quant_;
  }
  LOG_CODE_ERROR() << "dist is invalid " << dist;
  return 0;
}

} /* namespace selectivities */
} /* namespace niwa */
