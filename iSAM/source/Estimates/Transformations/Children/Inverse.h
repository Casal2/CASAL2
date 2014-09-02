/**
 * @file Inverse.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 3/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This transformation will perform: x = 1/x
 */
#ifndef ESTIMATES_TRANSFORMATIONSS_INVERSE_H_
#define ESTIMATES_TRANSFORMATIONSS_INVERSE_H_

// headers
#include "Estimates/Transformations/Transformation.h"

// namespaces
namespace isam {
namespace estimates {
namespace transformations {

/**
 * Class definition
 */
class Inverse : public estimates::Transformation {
public:
  // methods
  Inverse() = default;
  virtual                     ~Inverse() = default;
  Double                      Transform(Double initial_value) override final;
};

} /* namespace transformations */
} /* namespace estimates */
} /* namespace isam */

#endif /* ESTIMATES_TRANSFORMATIONSS_INVERSE_H_ */
