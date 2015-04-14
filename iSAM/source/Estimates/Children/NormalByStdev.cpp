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
namespace niwa {
namespace estimates {

/**
 * Default constructor
 */
NormalByStdev::NormalByStdev() {
  parameters_.Bind<Double>(PARAM_MU, &mu_, "Mu", "");
  parameters_.Bind<Double>(PARAM_SIGMA, &sigma_, "Sigma", "")->set_lower_bound(0.0, false);
}

/**
 * Calculate and return our score
 *
 * @return The score
 */
Double NormalByStdev::GetScore() {
  Double score_ = 0.5 * ((value() - mu_) / sigma_) * ((value() - mu_) / sigma_);
  return score_;
}

} /* namespace estimates */
} /* namespace niwa */
