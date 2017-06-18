/**
 * @file Beta.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Beta.h"

// namespaces
namespace niwa {
namespace additionalpriors {

/**
 * Default constructor
 *
 * Bind any parameters that are allowed to be loaded from the configuration files.
 * Set bounds on registered parameters
 * Register any parameters that can be an estimated or utilised in other run modes (e.g profiling, yields, projections etc)
 * Set some initial values
 *
 * Note: The constructor is parsed to generate Latex for the documentation.
 */
Beta::Beta(Model* model) : AdditionalPrior(model) {
  parameters_.Bind<Double>(PARAM_MU, &mu_, "Beta distribution mean (mu) parameter", "");
  parameters_.Bind<Double>(PARAM_SIGMA, &sigma_, "Beta distribution variance (sigma) parameter", "")
      ->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_A, &a_, "Beta distribution lower bound of the range (A) parameter", "");
  parameters_.Bind<Double>(PARAM_B, &b_, "Beta distribution upper bound of the range (B) parameter", "");
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when we cannot use bind<>() overloads
 *
 * Note: all parameters are populated from configuration files
 */
void Beta::DoValidate() {
  if (a_ >= b_)
    LOG_ERROR_P(PARAM_B) << "value (" << AS_DOUBLE(b_) << ") cannot be less than or equal to a (" << AS_DOUBLE(a_) << ")";
  Double max_sigma = ((((mu_ - a_) * (b_ - mu_)) / (sigma_ * sigma_)) - 1);
  if (max_sigma <= 0.0)
    LOG_ERROR_P(PARAM_SIGMA) << "value (" << AS_DOUBLE(sigma_) << ") is invalid. (" << mu_ << " - " << a_ << ") * ("
      << b_ << " - " << mu_ << ") / (" << sigma_ << " * " << sigma_ << ") - 1.0 == " << max_sigma << " <= 0.0";
}

/**
 * TODO: Add documentation
 */
Double Beta::GetScore() {
  Double value = 1.0;
  Double score_ = 0.0;
  v_ = (mu_ - a_) / (b_ - a_);
  t_ = (((mu_ - a_) * (b_ - mu_)) / (sigma_ * sigma_)) - 1.0;
  m_ = t_ * v_;
  n_ = t_ * (1.0 - v_);
  score_ = ((1.0 - m_) * log(value - a_)) + ((1.0 - n_) * log(b_ - value));
  return score_;
}

} /* namespace additionalpriors */
} /* namespace niwa */
