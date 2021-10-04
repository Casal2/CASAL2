/**
 * @file Normal.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 19/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
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
 * Register any parameters that can be an estimated or utilised in other run modes (e.g., profiling, yields, projections, etc.)
 * Set some initial values
 *
 * Note: The constructor is parsed to generate LaTeX for the documentation.
 */
Normal::Normal(shared_ptr<Model> model) : AgeingError(model) {
  parameters_.Bind<Double>(PARAM_CV, &cv_, "CV of the misclassification matrix", "")->set_lower_bound(0.0, false);
  parameters_
      .Bind<unsigned>(PARAM_K, &k_, "k defines the minimum age of individuals which can be misclassified, i.e., individuals of age less than k have no ageing error", "", 0u)
      ->set_lower_bound(0u);

  RegisterAsAddressable(PARAM_CV, &cv_);
}

/**
 * Populate any parameters,
 * Validate that values are within expected ranges when bind<>() overloads cannot be used
 *
 * Note: all parameters are populated from configuration files
 */
void Normal::DoValidate() {
  if (cv_ <= 0.0)
    LOG_ERROR_P(PARAM_CV) << "CV value (" << AS_DOUBLE(cv_) << ") cannot be less than or equal to 0.0";

  if (k_ > max_age_)
    LOG_ERROR_P(PARAM_K) << "K value (" << k_ << ") cannot be greater than the maximum age in the model (" << max_age_ << ")";
}

/**
 * Reset this object
 */
void Normal::DoBuild() {
  DoReset();
}

/**
 * Reset the mis_matrix to ensure that it has the latest
 * changes from any addressable modifications
 */
void Normal::DoReset() {
  unsigned age         = 0;
  Double min_age_class = 0.0;

  for (unsigned i = 0; i < age_spread_; ++i) {
    age = min_age_ + i;

    for (unsigned j = 0; j < age_spread_; ++j) {
      min_age_class = (min_age_ + j) - 0.5;

      if (j == 0)
        mis_matrix_[i][j] = NormalCDF(min_age_class + 1.0, age, age * cv_);
      else if (j == age_spread_ - 1 && plus_group_)
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

/*
TODO: Do this.
 # Normal ageing error
Expected<-c(10,20,30,20,15,5)
mMisMatrix<-Normal(min.age=3,max.age=8,dCV=0.1,iK=0,bAgePlusGroup=T)
round(mMisMatrix,5)
#        [,1]    [,2]    [,3]    [,4]    [,5]    [,6]
#[1,] 0.95221 0.04779 0.00000 0.00000 0.00000 0.00000
#[2,] 0.10565 0.78870 0.10556 0.00009 0.00000 0.00000
#[3,] 0.00135 0.15731 0.68269 0.15731 0.00135 0.00000
#[4,] 0.00002 0.00619 0.19612 0.59534 0.19612 0.00621
#[5,] 0.00000 0.00018 0.01588 0.22146 0.52495 0.23753
#[6,] 0.00000 0.00001 0.00088 0.02951 0.23559 0.73401
execute(Expected,mMisMatrix)
#11.675902 21.097643 26.756975 20.097275 13.015051  7.357153
mMisMatrix<-Normal(min.age=3,max.age=8,dCV=0.1,iK=4,bAgePlusGroup=T)
round(mMisMatrix,5)
#        [,1]    [,2]    [,3]    [,4]    [,5]    [,6]
#[1,] 1.00000 0.00000 0.00000 0.00000 0.00000 0.00000
#[2,] 0.10565 0.78870 0.10556 0.00009 0.00000 0.00000
#[3,] 0.00135 0.15731 0.68269 0.15731 0.00135 0.00000
#[4,] 0.00002 0.00619 0.19612 0.59534 0.19612 0.00621
#[5,] 0.00000 0.00018 0.01588 0.22146 0.52495 0.23753
#[6,] 0.00000 0.00001 0.00088 0.02951 0.23559 0.73401
execute(Expected,mMisMatrix)
#[1] 12.153806 20.619743 26.756972 20.097275 13.015051  7.357153
mMisMatrix<-Normal(min.age=3,max.age=8,dCV=0.1,iK=0,bAgePlusGroup=F)
round(mMisMatrix,5)
#        [,1]    [,2]    [,3]    [,4]    [,5]    [,6]
#[1,] 0.95221 0.04779 0.00000 0.00000 0.00000 0.00000
#[2,] 0.10565 0.78870 0.10556 0.00009 0.00000 0.00000
#[3,] 0.00135 0.15731 0.68269 0.15731 0.00135 0.00000
#[4,] 0.00002 0.00619 0.19612 0.59534 0.19612 0.00619
#[5,] 0.00000 0.00018 0.01588 0.22146 0.52495 0.22146
#[6,] 0.00000 0.00001 0.00088 0.02951 0.23559 0.46803
execute(Expected,mMisMatrix)
#[1] 11.675902 21.097643 26.756975 20.097275 13.015051  5.785982
mMisMatrix<-Normal(min.age=3,max.age=8,dCV=0.1,iK=4,bAgePlusGroup=F)
round(mMisMatrix,5)
#        [,1]    [,2]    [,3]    [,4]    [,5]    [,6]
#[1,] 1.00000 0.00000 0.00000 0.00000 0.00000 0.00000
#[2,] 0.10565 0.78870 0.10556 0.00009 0.00000 0.00000
#[3,] 0.00135 0.15731 0.68269 0.15731 0.00135 0.00000
#[4,] 0.00002 0.00619 0.19612 0.59534 0.19612 0.00619
#[5,] 0.00000 0.00018 0.01588 0.22146 0.52495 0.22146
#[6,] 0.00000 0.00001 0.00088 0.02951 0.23559 0.46803
execute(Expected,mMisMatrix)
#[1] 12.153806 20.619743 26.756972 20.097275 13.015051  5.785982
*/

} /* namespace ageingerrors */
} /* namespace niwa */
