/**
 * @file LogOdds.h
 * @author  C.Marsh
 * @date 2/10/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This transformation will transform X, where a < X < b
 * to Y which is un-constrained by bounds. This follows from the R-Stan documenr page 444
 * http://mc-stan.org/documentation/
 * The Jocobian for a scalar is the calculated as the absolute derivative of the Inverse transform
 */
#ifndef ESTIMATES_TRANSFORMATIONSS_LOGODDS_H_
#define ESTIMATES_TRANSFORMATIONSS_LOGODDS_H_

// headers
#include "Estimates/Transformations/Transformation.h"

// namespaces
namespace niwa {
namespace estimates {
namespace transformations {

/**
 * Class definition
 */
class LogOdds : public estimates::Transformation {
public:
  // methods
  LogOdds() = default;
  virtual                     ~LogOdds() = default;
  Double                      Transform(Double initial_value) override final;
  Double                      Untransform(Double initial_value) override final;
//  Double                      Jacobian(Double initial_value) override final;

private:
  //members
  Double                      lower_bound_;
  Double                      upper_bound_;

};

} /* namespace transformations */
} /* namespace estimates */
} /* namespace niwa */

#endif /* ESTIMATES_TRANSFORMATIONSS_LOGODDS_H_ */
