/**
 * @file DoubleNormal.cpp
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
#include "DoubleNormal.h"

#include <boost/math/distributions/lognormal.hpp>
#include <cmath>

#include "AgeLengths/AgeLength.h"
#include "Model/Model.h"
#include "TimeSteps/Manager.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Explicit constructor
 */
DoubleNormal::DoubleNormal(Model* model)
  : Selectivity(model) {

  parameters_.Bind<Double>(PARAM_MU, &mu_, "Mu", "");
  parameters_.Bind<Double>(PARAM_SIGMA_L, &sigma_l_, "Sigma L", "");
  parameters_.Bind<Double>(PARAM_SIGMA_R, &sigma_r_, "Sigma R", "");
  parameters_.Bind<Double>(PARAM_ALPHA, &alpha_, "Alpha", "", 1.0);

  RegisterAsEstimable(PARAM_MU, &mu_);
  RegisterAsEstimable(PARAM_SIGMA_L, &sigma_l_);
  RegisterAsEstimable(PARAM_SIGMA_R, &sigma_r_);
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
void DoubleNormal::DoValidate() {
  if (alpha_ <= 0.0)
    LOG_ERROR_P(PARAM_MU) << ": alpha cannot be less than or equal to 0.0";
  if (sigma_l_ <= 0.0)
    LOG_ERROR_P(PARAM_SIGMA_L) << ": sigma_l cannot be less than or equal to 0.0";
  if (sigma_r_ <= 0.0)
    LOG_ERROR_P(PARAM_SIGMA_R) << ": sigmal_r cannot be less than or equal to 0.0";
}

/**
 * Reset this selectivity so it's ready for the next execution
 * phase in the model.
 *
 * This method will rebuild the cache of selectivity values
 * for each age in the model.
 */
void DoubleNormal::Reset() {
  for (unsigned age = model_->min_age(); age <= model_->max_age(); ++age) {
	  Double temp = (Double)age;
    if (temp < mu_)
      values_[age] = pow(2.0, -((temp - mu_) / sigma_l_ * (temp - mu_) / sigma_l_)) * alpha_;
    else
      values_[age] = pow(2.0, -((temp - mu_)/sigma_r_ * (temp - mu_) / sigma_r_)) * alpha_;
  }
}
/**
 * GetLengthBasedResult function
 *
 * @param age
 * @param age_length AgeLength pointer
 * @return Double selectivity for an age based on age length distribution
 */

Double DoubleNormal::GetLengthBasedResult(unsigned age, AgeLength* age_length) {
  unsigned year = model_->current_year();
  unsigned time_step = model_->managers().time_step()->current_time_step();
  Double cv = age_length->cv(year, age, time_step);
  Double mean = age_length->mean_length(year, age);
  string dist = age_length->distribution();

  if (dist == PARAM_NONE || n_quant_ <= 1) {

    if (mean < mu_)
      return pow(2.0, -((mean - mu_) / sigma_l_ * (mean - mu_) / sigma_l_)) * alpha_;
    else
      return  pow(2.0, -((mean - mu_)/sigma_r_ * (mean - mu_) / sigma_r_)) * alpha_;

  } else if (dist == PARAM_NORMAL) {

    Double sigma = cv * mean;
    Double size = 0.0;
    Double total = 0.0;

    for (unsigned j = 0; j < n_quant_; ++j) {
      size = mean + sigma * quantiles_at_[j];

      if (size < mu_)
        total +=  pow(2.0, -((size - mu_) / sigma_l_ * (size - mu_) / sigma_l_)) * alpha_;
      else
        total +=   pow(2.0, -((size - mu_)/sigma_r_ * (size - mu_) / sigma_r_)) * alpha_;
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

      if (size < mu_)
        total +=  pow(2.0, -((size - mu_) / sigma_l_ * (size - mu_) / sigma_l_)) * alpha_;
      else
        total +=   pow(2.0, -((size - mu_)/sigma_r_ * (size - mu_) / sigma_r_)) * alpha_;
    }
    return total / n_quant_;
  }
  LOG_CODE_ERROR() << "dist is invalid " << dist;
  return 0;
}

} /* namespace selectivities */
} /* namespace niwa */
