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
Lognormal::Lognormal(shared_ptr<Model> model) : Estimate(model) {
  parameters_.Bind<Double>(PARAM_MU, &mu_, "The lognormal prior mean (mu) parameter", "")->set_lower_bound(0.0, false);
  parameters_.Bind<Double>(PARAM_CV, &cv_, "The lognormal variance (cv) parameter", "")->set_lower_bound(0.0, false);

  RegisterAsAddressable(PARAM_MU, &mu_);
  RegisterAsAddressable(PARAM_CV, &cv_);
}

/**
 * Calculate and return the score
 * @return The score
 */
Double Lognormal::GetScore() {
  sigma_        = sqrt(log(1 + cv_ * cv_));
  Double score_ = log(value()) + 0.5 * pow(log(value() / mu_) / sigma_ + sigma_ * 0.5, 2);
  return score_;
}
/**
 * GetPriorValues()
 *
 * @return vector of mu and sigma
 */
vector<Double>   Lognormal::GetPriorValues() {
  vector<Double> result = {mu_, cv_};
  return result;
}
vector<string>   Lognormal::GetPriorLabels() {
  vector<string> result = {PARAM_MU, PARAM_CV};
  return result;
}
} /* namespace estimates */
} /* namespace niwa */
