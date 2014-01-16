/**
 * @file NormalByStdev.cpp
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
#include "NormalByStdev.h"

// Namespaces
namespace isam {
namespace estimates {

/**
 * Default constructor
 */
NormalByStdev::NormalByStdev() {
  parameters_.Bind<double>(PARAM_MU, &mu_, "Mu");
  parameters_.Bind<double>(PARAM_SIGMA, &sigma_, "Sigma");
}

/**
 * Validate parameters from the configuration file
 */
void NormalByStdev::DoValidate() {
  if (sigma_ <= 0.0)
    LOG_ERROR(parameters_.location(PARAM_SIGMA) << ": sigma (" << AS_DOUBLE(sigma_) << ") cannot be less than or equal to 0.0");
}

/**
 * Calculate and return our score
 *
 * @param param The value to use in the calculation
 * @return The score
 */
Double NormalByStdev::GetScore(Double param) {
  score_ = 0.5 * ((param - mu_) / sigma_) * ((param - mu_) / sigma_);
  return score_;
}

} /* namespace estimates */
} /* namespace isam */
