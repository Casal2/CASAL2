/**
 * @file LogOdds.cpp
 * @author  C Marsh
 * @date 3/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "LogOdds.h"

// namespaces
namespace niwa {
namespace estimates {
namespace transformations {

/**
 * @param initial_value The value we want to transform
 * @param upper_bound upper_bound of initial value
 * @param lower_bound lower bound of initial value
 * @return The transformed value
 */
Double LogOdds::Transform(Double initial_value) {
  Double result = 0;
  if (upper_bound_ != lower_bound_) {
    Double u = (initial_value - lower_bound_) / (upper_bound_ - lower_bound_);
    result = log(u/(1-u));
  }
  return result;
}

/**
 * @param initial_value The value we want to transform
 * @param upper_bound upper_bound of initial value
 * @param lower_bound lower bound of initial value
 * @return The untransformed value
 */

Double LogOdds::Untransform(Double initial_value) {
  Double result = 0;
  if (upper_bound_ != lower_bound_) {
    result = lower_bound_ + (upper_bound_ - lower_bound_) * 1 / (1 + exp( -initial_value));
  }
  return result;
}

  /**
   * @param initial_value The value we want to transform
   * @param upper_bound upper_bound of initial value
   * @param lower_bound lower bound of initial value
   * @return The jocobian to be added to the likeihood associated with initial variable
   */

/*
Double LogOdds::Jacobian(Double initial_value) {
  Double result = 0;
  result = (upper_bound_ - lower_bound_) * exp(initial_value) / pow(1 + exp(initial_value ), 2);
  return result;
}
*/

} /* namespace transformations */
} /* namespace estimates */
} /* namespace niwa */
