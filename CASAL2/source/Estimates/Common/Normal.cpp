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
Normal::Normal(shared_ptr<Model> model) : Estimate(model) {
  parameters_.Bind<Double>(PARAM_MU, &mu_, "The normal prior mean (mu) parameter", "");
  parameters_.Bind<Double>(PARAM_CV, &cv_, "The normal variance (standard devation) parameter", "")->set_lower_bound(0.0, false);

  RegisterAsAddressable(PARAM_MU, &mu_);
  RegisterAsAddressable(PARAM_CV, &cv_);
}

/**
 * Calculate and return the score
 * @return The score
 */
Double Normal::GetScore() {
  Double score_ = 0.5 * ((value() - mu_) / (cv_ * mu_)) * ((value() - mu_) / (cv_ * mu_));
  return score_;
}
/**
 * GetPriorValues()
 *
 * @return vector of mu and sigma
 */
vector<Double>   Normal::GetPriorValues() {
  vector<Double> result = {mu_, cv_};
  return result;
}
vector<string>   Normal::GetPriorLabels() {
  vector<string> result = {PARAM_MU, PARAM_CV};
  return result;
}
} /* namespace estimates */
} /* namespace niwa */
