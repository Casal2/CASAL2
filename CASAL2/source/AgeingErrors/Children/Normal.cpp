/**
 * @file Normal.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 19/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "Normal.h"

#include <boost/math/distributions/normal.hpp>

// mamespaces
namespace niwa {
namespace ageingerrors {

/**
 * Normal Distribution CDF Method
 *
 * @param x X value
 * @param mu Mu value
 * @param sigma Sigma value
 * @return Normal CDF
 */

Double NormalCDF(Double x, Double mu, Double sigma) {
  if (sigma <= 0.0 && x < mu)
    return 0;
  else if (sigma <= 0.0 && x >= mu)
    return 1;

  boost::math::normal s(AS_DOUBLE(mu), AS_DOUBLE(sigma));
  return cdf(s, AS_DOUBLE(x));
}


/**
 * Default constructor
 *
 * Bind any parameters that are allowed to be loaded from the configuration files.
 * Set bounds on registered parameters
 * Register any parameters that can be an estimated or utilised in other run modes (e.g profiling, yields, projections etc)
 * Set some initial values
 *
 * Note: The constructor is parsed to generate Latex for the documentation.
 */
Normal::Normal(Model* model) : AgeingError(model) {
  parameters_.Bind<Double>(PARAM_CV, &cv_, "CV for Misclassification matrix", "")->set_lower_bound(0.0);
  parameters_.Bind<unsigned>(PARAM_K, &k_, "TBA", "", 0u);

  RegisterAsEstimable(PARAM_CV, &cv_);
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when we cannot use bind<>() overloards
 *
 * Note: all parameters are populated from configuration files
 */
void Normal::DoValidate() {
  if (cv_ <= 0.0)
    LOG_ERROR_P(PARAM_CV) << "value (" << AS_DOUBLE(cv_) << ") cannot be less than or equal to 0.0";
  if (k_ > max_age_)
    LOG_ERROR_P(PARAM_K) << "value (" << k_ << ") cannot be greater than the model's max age (" << max_age_ << ")";
}

/**
 * Reset this object for use
 */
void Normal::DoBuild() {
  DoReset();
}

/**
 * Reset our mis_matrix to ensure it has the latest
 * changes from any estimable modifications
 */
void Normal::DoReset() {
  Double age = 0.0;
  Double min_age_class = 0.0;

  for (unsigned i = 0; i < age_spread_; ++i) {
    age = min_age_ + i;

    for (unsigned j = 0; j < age_spread_; ++j) {
      min_age_class = (min_age_ + j) - 0.5;

      if (j == 0)
        mis_matrix_[i][j] = NormalCDF(min_age_class + 1.0, age, age * cv_);
      else if (j == age_spread_ - 1 && age_plus_)
        mis_matrix_[i][j] = 1.0 - NormalCDF(min_age_class, age, age * cv_);
      else
        mis_matrix_[i][j] = NormalCDF(min_age_class + 1.0, age, age * cv_) - NormalCDF(min_age_class, age, age * cv_);
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
} /* namespace niwa */
