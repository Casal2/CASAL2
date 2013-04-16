/**
 * @file Normal.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 16/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Normal.h"

#include <boost/math/distributions/normal.hpp>

// Namespaces
namespace isam {
namespace ageingerrors {

/**
 * Normal Distribution CDF Method
 */
Double NormalCDF(Double x, Double mu, Double sigma) {
  if (sigma <= 0.0 && x < mu)
    return 0;
  else if (sigma <= 0.0 && x >= mu)
    return 1;

  boost::math::normal s(mu, sigma);
  return cdf(s, x);
}


/**
 * Default Constructor
 */
Normal::Normal() {
  parameters_.RegisterAllowed(PARAM_CV);
  parameters_.RegisterAllowed(PARAM_K);

  RegisterAsEstimable(PARAM_CV, &cv_);
}

/**
 * Validate the parameters from our configuration file
 */
void Normal::Validate() {
  AgeingError::Validate();

  CheckForRequiredParameter(PARAM_CV);

  cv_ = parameters_.Get(PARAM_CV).GetValue<double>();
  k_  = parameters_.Get(PARAM_K).GetValue<unsigned>(0);

  if (cv_ <= 0.0)
    LOG_ERROR(parameters_.location(PARAM_CV) << ": cv (" << cv_ << ") cannot be less than or equal to 0.0");
  if (k_ > max_age_)
    LOG_ERROR(parameters_.location(PARAM_K) << ": k (" << k_ << ") cannot be greater than the model's max age (" << max_age_ << ")");
}

/**
 * Build runtime objects for the normal
 * ageing error. In this case we'll just
 * call the parent build then do a reset
 */
void Normal::Build() {
  AgeingError::Build();
  Reset();
}

/**
 * Reset our mis_matrix to ensure it has the latest
 * changes from any estimable modifications
 */
void Normal::Reset() {

  Double age = 0.0;
  Double min_age_class = 0.0;

  for (unsigned i = 0; i < age_spread_; ++i) {
    age = min_age_ + i;

    for (unsigned j = 0; j < age_spread_; ++j) {
      min_age_class = (min_age_ + j) - 0.5;

      if (j == 0)
        mis_matrix_[i][j] = NormalCDF(min_age_class + 1, age, age * cv_);
      else if (j == age_spread_ - 1 && age_plus_)
        mis_matrix_[i][j] = NormalCDF(min_age_class, age, age * cv_);
      else
        mis_matrix_[i][j] = NormalCDF(min_age_class + 1, age, age * cv_) - NormalCDF(min_age_class, age, age * cv_);
    }
  }

  if (k_ > min_age_) {
    for (unsigned i = 0; i < k_ - min_age_; ++i) {
      for (unsigned j = 0; j < k_ - min_age_; ++j) {
        mis_matrix_[i][j] = 0.0;
      }
      mis_matrix_[i][i] = 1.0;
    }
  }
}

} /* namespace ageingerrors */
} /* namespace isam */
