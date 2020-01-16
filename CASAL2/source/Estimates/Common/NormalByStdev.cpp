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
NormalByStdev::NormalByStdev(Model* model) : Estimate(model) {
  parameters_.Bind<Double>(PARAM_MU, &mu_, "The normal prior mean (mu) parameter", "");
  parameters_.Bind<Double>(PARAM_SIGMA, &sigma_, "The normal variance (standard devation) parameter", "")->set_lower_bound(0.0, false);
  parameters_.Bind<bool>(PARAM_LOGNORMAL_TRANSFORMATION, &assume_lognormal_, "Add a jacobian if the derived outcome of the estimate is assumed to be lognormal, e.g., used for rectuitment deviations in the recruitment process. See the User Manual for more information", "", false);


  RegisterAsAddressable(PARAM_MU, &mu_);
  RegisterAsAddressable(PARAM_SIGMA, &sigma_);
}

/**
 * Calculate and return our score
 *
 * @return The score
 */
Double NormalByStdev::GetScore() {
  Double score_ = 0.5 * ((value() - mu_) / sigma_) * ((value() - mu_) / sigma_);
  if (assume_lognormal_) {
    score_+= value() - (0.5 * sigma_ * sigma_);
  }
  return score_;
}

} /* namespace estimates */
} /* namespace niwa */
