/**
 * @file Transformation.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 3/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is a modifier that can be applied to an estimate. This allows
 * the estimate to be modified before it's sent off to the minimiser.
 */
#ifndef ESTIMATES_TRANSFORMATION_H_
#define ESTIMATES_TRANSFORMATION_H_

// headers
#include <boost/shared_ptr.hpp>
#include <string>

#include "Utilities/Types.h"

// namespaces
namespace isam {
namespace estimates {

using isam::utilities::Double;
using std::string;

/**
 * class definition
 */
class Transformation {
public:
  Transformation();
  virtual ~Transformation() = default;

  virtual Double              Transform(Double initial_value) = 0;
};

} /* namespace estimates */
} /* namespace isam */

typedef boost::shared_ptr<isam::estimates::Transformation> EstimateTransformationPtr;

#endif /* ESTIMATES_TRANSFORMATION_H_ */
