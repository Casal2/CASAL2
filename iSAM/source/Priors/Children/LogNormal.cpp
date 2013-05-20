/*
 * LogNormal.cpp
 *
 *  Created on: 6/03/2013
 *      Author: Admin
 */

#include "LogNormal.h"

namespace isam {
namespace priors {

/**
 * Default constructor
 */
LogNormal::LogNormal() {
  parameters_.RegisterAllowed(PARAM_MU);
  parameters_.RegisterAllowed(PARAM_CV);
}

/**
 * Destructor
 */
LogNormal::~LogNormal() noexcept(true) {
}

/**
 * Validate the parameters from the configuration file
 */
void LogNormal::Validate() {
  Prior::Validate();

  CheckForRequiredParameter(PARAM_MU);
  CheckForRequiredParameter(PARAM_CV);

  mu_ = parameters_.Get(PARAM_MU).GetValue<double>();
  cv_ = parameters_.Get(PARAM_CV).GetValue<double>();

  if (mu_ <= 0.0)
    LOG_ERROR(parameters_.location(PARAM_MU) << ": mu (" << AS_DOUBLE(mu_) << ") cannot be less than or equal to 0.0");
  if (cv_ <= 0.0)
    LOG_ERROR(parameters_.location(PARAM_CV) << ": cv (" << AS_DOUBLE(cv_) << ") cannot be less than or equal to 0.0");
}

/**
 * Calculate and return the score
 *
 * @param param The parameter to use for calculation
 * @return The score for this prior
 */
Double LogNormal::GetScore(Double param) {
  sigma_ = sqrt(log( 1 + cv_ * cv_));
  score_ = log(param) + 0.5 * pow(log(param / mu_) / sigma_ * sigma_ * 0.5, 2);
  return score_;
}

} /* namespace priors */
} /* namespace isam */
