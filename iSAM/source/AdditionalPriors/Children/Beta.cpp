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
 */
Beta::Beta() {
  parameters_.Bind<Double>(PARAM_MU, &mu_, "Mu", "");
  parameters_.Bind<Double>(PARAM_SIGMA, &sigma_, "Sigma", "");
  parameters_.Bind<Double>(PARAM_A, &a_, "A", "");
  parameters_.Bind<Double>(PARAM_B, &b_, "B", "");
}

/**
 *
 */
void Beta::DoValidate() {
  if (sigma_ <= 0.0)
    LOG_ERROR(parameters_.location(PARAM_SIGMA) << ": sigma (" << AS_DOUBLE(sigma_) << ") cannot be less than or equal to 0.0");
  if (a_ >= b_)
    LOG_ERROR(parameters_.location(PARAM_B) << ": b (" << AS_DOUBLE(b_) << ") cannot be less than or equal to a (" << AS_DOUBLE(a_) << ")");
  if ( ((((mu_ - a_) * (b_ - mu_)) / (sigma_ * sigma_)) - 1) <= 0.0)
    LOG_ERROR(parameters_.location(PARAM_SIGMA) << " sigma (" << AS_DOUBLE(sigma_) << ") is too large");
}

/**
 *
 */
Double Beta::GetRatioScore() {
  return 0.0f;
}

/**
 *
 */
Double Beta::GetDifferenceScore() {
  return 0.0;
}

/**
 *
 */
Double Beta::GetMeanScore() {
  return 0.0;
}

} /* namespace additionalpriors */
} /* namespace niwa */
