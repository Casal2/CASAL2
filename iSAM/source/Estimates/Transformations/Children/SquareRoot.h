/**
 * @file SquareRoot.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 3/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This transformation will apply a sqrt(x)
 */
#ifndef ESTIMATES_TRANSFORMATIONS_SQUAREROOT_H_
#define ESTIMATES_TRANSFORMATIONS_SQUAREROOT_H_

// headers
#include "Estimates/Transformations/Transformation.h"

// namespaces
namespace isam {
namespace estimates {
namespace transformations {

/**
 * Class definition
 */
class SquareRoot : public estimates::Transformation {
public:
  SquareRoot() = default;
  virtual                     ~SquareRoot() = default;
  Double                      Transform(Double initial_value) override final;
  Double                      Untransform(Double initial_value) override final;
};

} /* namespace transformations */
} /* namespace estimates */
} /* namespace isam */

#endif /* SQUAREROOT_H_ */
