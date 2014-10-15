/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 3/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This factory will create the Estimate Transformations
 */
#ifndef ESTIMATES_TRANSFORMATIONS_FACTORY_H_
#define ESTIMATES_TRANSFORMATIONS_FACTORY_H_

// headers
#include "Estimates/Transformations/Transformation.h"

// namespaces
namespace isam {
namespace estimates {
namespace transformations {

/**
 * Class definition
 */
class Factory {
public:
  // methods
  static TransformationPtr Create(const string& sub_type);
private:
  // methods
  Factory() = delete;
  virtual ~Factory() = delete;
};

} /* namespace transformations */
} /* namespace estimates */
} /* namespace isam */

#endif /* ESTIMATES_TRANSFORMATIONS_FACTORY_H_ */
