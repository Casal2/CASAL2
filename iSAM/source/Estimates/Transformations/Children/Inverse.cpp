/**
 * @file Inverse.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 3/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Inverse.h"

// namespaces
namespace isam {
namespace estimates {
namespace transformations {

/**
 * Return the inverse of our value x = 1/1
 *
 * @param initial_value The value we want to transform
 * @return The transformed value
 */
Double Inverse::Transform(Double initial_value) {
  Double result = initial_value == 0 ? 0 : 1 / initial_value;
  return result;
}

/**
 *
 */
Double Inverse::Untransform(Double initial_value) {
  Double result = initial_value == 0 ? 0 : 1 / initial_value;
  return result;
}

} /* namespace transformations */
} /* namespace estimates */
} /* namespace isam */
