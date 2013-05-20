/**
 * @file NormalLog.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 8/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "NormalLog.h"

// Namespaces
namespace isam {
namespace priors {

/**
 * Default constructor
 */
NormalLog::NormalLog() {
  parameters_.RegisterAllowed(PARAM_MU);
  parameters_.RegisterAllowed(PARAM_SIGMA);
}

/**
 * Destructor
 */
NormalLog::~NormalLog() noexcept(true) {
}

/**
 * Validate the parameters from the configuration file
 */
void NormalLog::Validate() {
  Prior::Validate();

  CheckForRequiredParameter(PARAM_MU);
  CheckForRequiredParameter(PARAM_SIGMA);

  mu_     = parameters_.Get(PARAM_MU).GetValue<double>();
  sigma_  = parameters_.Get(PARAM_SIGMA).GetValue<double>();

  if (sigma_ <= 0.0)
    LOG_ERROR(parameters_.location(PARAM_SIGMA) << ": sigma (" << AS_DOUBLE(sigma_) << ") cannot be less than or equal to 0.0");
}

/**
 * Calculate and return the score for this prior
 *
 * @param param The parameter to use in calculations
 * @return The score
 */
Double NormalLog::GetScore(Double param) {
  score_ = log(param) + 0.5 * pow((log(param) - mu_) / sigma_, 2);
  return score_;
}

} /* namespace priors */
} /* namespace isam */
