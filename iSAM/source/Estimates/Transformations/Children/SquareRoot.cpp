/**
 * @file SquareRoot.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 3/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "SquareRoot.h"

// namespaces
namespace isam {
namespace estimates {
namespace transformations {

/**
 * Apply a square root x = sqrt(x) transformation
 * to our estimate
 *
 * @param initial_value The initial value to transform
 * @return The transformed value
 */
Double SquareRoot::Transform(Double initial_value) {
  return ::sqrt(initial_value);
}

} /* namespace transformations */
} /* namespace estimates */
} /* namespace isam */
