/*
 * Log.cpp
 *
 *  Created on: 3/09/2014
 *      Author: Admin
 */

#include "Log.h"

namespace niwa {
namespace estimates {
namespace transformations {

/**
 * Transform the initial value parameter
 *
 * @param initial_value The value to transform
 * @return The transformed value
 */
Double Log::Transform(Double initial_value) {
  return log(initial_value);
}

/**
 *
 */
Double Log::Untransform(Double initial_value) {
  return exp(initial_value);
}

} /* namespace transformations */
} /* namespace estimates */
} /* namespace niwa */
