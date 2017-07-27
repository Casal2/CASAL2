/**
 * @file Beta.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 6/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Beta.h"

// Namespaces
namespace niwa {
namespace estimates {

/**
 * Default constructor
 */
Beta::Beta(Model* model) : Estimate(model) {
  parameters_.Bind<Double>(PARAM_MU, &mu_, "Beta prior  mean (mu) parameter", "");
  parameters_.Bind<Double>(PARAM_SIGMA, &sigma_, "Beta prior variance (sigma) parameter", "")->set_lower_bound(0.0, false);
  parameters_.Bind<Double>(PARAM_A, &a_, "Beta prior lower bound of the range (A) parameter", "");
  parameters_.Bind<Double>(PARAM_B, &b_, "Beta prior upper bound of the range (B) parameter", "");

  RegisterAsAddressable(PARAM_MU, &mu_);
  RegisterAsAddressable(PARAM_SIGMA, &sigma_);
}

/**
 * Validate the parameters from the configuration file
 */
void Beta::DoValidate() {
  if (a_ >= b_)
    LOG_ERROR_P(PARAM_B) << "(" << AS_DOUBLE(b_) << ") cannot be less than or equal to a (" << AS_DOUBLE(a_) << ")";
  if ( ((((mu_ - a_) * (b_ - mu_)) / (sigma_ * sigma_)) - 1) <= 0.0)
    LOG_ERROR_P(PARAM_SIGMA) << "(" << AS_DOUBLE(sigma_) << ") is too large";
}

/**
 * Get the score
 *
 * @return The score for this prior
 */
Double Beta::GetScore() {
  Double score_ = 0.0;
  v_ = (mu_ - a_) / (b_ - a_);
  t_ = (((mu_ - a_) * (b_ - mu_)) / (sigma_ * sigma_)) - 1.0;
  m_ = t_ * v_;
  n_ = t_ * (1.0 - v_);
  score_ = ((1.0 - m_) * log(value() - a_)) + ((1.0 - n_) * log(b_ - value()));

  return score_;
}

} /* namespace estimates */
} /* namespace niwa */
