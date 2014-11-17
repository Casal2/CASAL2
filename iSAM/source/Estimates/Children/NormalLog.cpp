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
namespace estimates {

/**
 * Default constructor
 */
NormalLog::NormalLog() {
  parameters_.Bind<Double>(PARAM_MU, &mu_, "Mu", "");
  parameters_.Bind<Double>(PARAM_SIGMA, &sigma_, "Sigma", "");
}

/**
 * Validate the parameters from the configuration file
 */
void NormalLog::DoValidate() {
  if (sigma_ <= 0.0)
    LOG_ERROR(parameters_.location(PARAM_SIGMA) << ": sigma (" << AS_DOUBLE(sigma_) << ") cannot be less than or equal to 0.0");
}

/**
 * Calculate and return the score for this prior
 *
 * @return The score
 */
Double NormalLog::GetScore() {
  Double score_ = log(value()) + 0.5 * pow((log(value()) - mu_) / sigma_, 2);
  return score_;
}

} /* namespace estimates */
} /* namespace isam */
