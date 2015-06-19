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
Beta::Beta() {
  parameters_.Bind<Double>(PARAM_MU, &mu_, "Mu", "");
  parameters_.Bind<Double>(PARAM_SIGMA, &sigma_, "Sigma", "")
      ->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_A, &a_, "A", "");
  parameters_.Bind<Double>(PARAM_B, &b_, "B", "");
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
  if ( ((((mu_ - a_) * (b_ - mu_)) / (sigma_ * sigma_)) - 1) <= 0.0)
    LOG_ERROR_P(PARAM_SIGMA) << "value (" << AS_DOUBLE(sigma_) << ") is too large";
}

/**
 * TODO: Add documentation
 */
Double Beta::GetRatioScore() {
  return 0.0f;
}

/**
 * TODO: Add documentation
 */
Double Beta::GetDifferenceScore() {
  return 0.0;
}

/**
 * TODO: Add documentation
 */
Double Beta::GetMeanScore() {
  return 0.0;
}

} /* namespace additionalpriors */
} /* namespace niwa */
