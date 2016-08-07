/**
 * @file Normal.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 8/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Normal.h"

// Namespaces
namespace niwa {
namespace estimates {

/**
 * Default constructor
 */
Normal::Normal() {
  parameters_.Bind<Double>(PARAM_MU, &mu_, "The normal prior mean (mu) parameter", "");
  parameters_.Bind<Double>(PARAM_CV, &cv_, "The normal prior variance (CV) parameter", "")->set_lower_bound(0.0, false);

  RegisterAsEstimable(PARAM_MU, &mu_);
  RegisterAsEstimable(PARAM_CV, &cv_);
}

/**
 * Calculate and return the score for this prior
 */
Double Normal::GetScore() {
  Double score_ = 0.5 * ((value() - mu_) / (cv_ * mu_)) * ((value() - mu_) / (cv_ * mu_));
  return score_;
}

} /* namespace estimates */
} /* namespace niwa */
