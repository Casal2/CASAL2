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
namespace niwa {
namespace estimates {

/**
 * Default constructor
 */
NormalLog::NormalLog(Model* model) : Estimate(model) {
  parameters_.Bind<Double>(PARAM_MU, &mu_, "The normal-log prior mean (mu) parameter", "");
  parameters_.Bind<Double>(PARAM_SIGMA, &sigma_, "The normal-log prior variance (standard deviation) parameter", "")->set_lower_bound(0.0, false);

  RegisterAsEstimable(PARAM_MU, &mu_);
  RegisterAsEstimable(PARAM_SIGMA, &sigma_);
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
} /* namespace niwa */
