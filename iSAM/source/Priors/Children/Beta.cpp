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
namespace isam {
namespace priors {

/**
 * Default constructor
 */
Beta::Beta() {
  parameters_.RegisterAllowed(PARAM_M);
  parameters_.RegisterAllowed(PARAM_SIGMA);
  parameters_.RegisterAllowed(PARAM_A);
  parameters_.RegisterAllowed(PARAM_B);
}

/**
 * Destructor
 */
Beta::~Beta() noexcept(true) {
}

/**
 * Validate the parameters from the configuration file
 */
void Beta::Validate() {
  Prior::Validate();

  CheckForRequiredParameter(PARAM_MU);
  CheckForRequiredParameter(PARAM_SIGMA);
  CheckForRequiredParameter(PARAM_A);
  CheckForRequiredParameter(PARAM_B);

  mu_     = parameters_.Get(PARAM_MU).GetValue<double>();
  sigma_  = parameters_.Get(PARAM_SIGMA).GetValue<double>();
  a_      = parameters_.Get(PARAM_A).GetValue<double>();
  b_      = parameters_.Get(PARAM_B).GetValue<double>();

  if (sigma_ <= 0.0)
    LOG_ERROR(parameters_.location(PARAM_SIGMA) << ": sigma (" << sigma_ << ") cannot be less than or equal to 0.0");
  if (a_ >= b_)
    LOG_ERROR(parameters_.location(PARAM_B) << ": b (" << b_ << ") cannot be less than or equal to a (" << a_ << ")");
  if ( ((((mu_ - a_) * (b_ - mu_)) / (sigma_ * sigma_)) - 1) <= 0.0)
    LOG_ERROR(parameters_.location(PARAM_SIGMA) << " sigma (" << sigma_ << ") is too large");
}

/**
 * Get the score
 *
 * @param param The parameter to use in the calculation
 * @return The score for this prior
 */
Double Beta::GetScore(Double param) {
  score_ = 0.0;

  v_ = (mu_ - a_) / (b_ - a_);
  t_ = (((mu_ - a_) * (b_ - mu_)) / (sigma_ * sigma_)) - 1.0;
  m_ = t_ * v_;
  n_ = t_ * (1.0 * v_);
  score_ = ((1.0 - m_) * log(param - a_)) + ((1.0 - n_) * log(b_ - param));

  return score_;
}

} /* namespace priors */
} /* namespace isam */
