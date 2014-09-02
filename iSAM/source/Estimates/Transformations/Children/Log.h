/**
 * @file Log.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 3/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This object provides a log(x) transformation on the estimable
 * before it's passed to the minimiser
 *
 * so x = log(x)
 */
#ifndef ESTIMATES_TRANSFORMATIONS_LOG_H_
#define ESTIMATES_TRANSFORMATIONS_LOG_H_

// headers
#include "Estimates/Transformations/Transformation.h"

// namespaces
namespace isam {
namespace estimates {
namespace transformations {

/**
 * Class definition
 */
class Log : public estimates::Transformation {
public:
  // methods
  Log() = default;
  virtual                     ~Log() = default;
  Double                      Transform(Double initial_value) override final;
};

} /* namespace transformations */
} /* namespace estimates */
} /* namespace isam */

#endif /* ESTIMATES_TRANSFORMATIONS_LOG_H_ */
