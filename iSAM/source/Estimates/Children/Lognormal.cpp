/*
 * LogNormal.cpp
 *
 *  Created on: 6/03/2013
 *      Author: Admin
 */

#include "Lognormal.h"

namespace niwa {
namespace estimates {

/**
 * Default constructor
 */
Lognormal::Lognormal() {
  parameters_.Bind<Double>(PARAM_MU, &mu_, "Mu", "")->set_lower_bound(0.0, false);
  parameters_.Bind<Double>(PARAM_CV, &cv_, "Cv", "")->set_lower_bound(0.0, false);
}

/**
 * Calculate and return the score
 *
 * @return The score for this prior
 */
Double Lognormal::GetScore() {
  sigma_ = sqrt(log( 1 + cv_ * cv_));
  Double score_ = log(value()) + 0.5 * pow(log(value() / mu_) / sigma_ + sigma_ * 0.5, 2);
  return score_;
}

} /* namespace estimates */
} /* namespace niwa */
