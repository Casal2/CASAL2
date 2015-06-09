/**
 * @file Francis.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 5/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "Francis.h"

// namespaces
namespace niwa {
namespace observations {
namespace dataweights {

/**
 *
 */
Double Francis::AdjustErrorValue(Double error_value, Double process_error) {
  return  error_value * weight_value_;
}

/**
 *
 */
Double Francis::AdjustLikelihood(Double likelihood) {
  return likelihood;
}

/**
 *
 */
bool Francis::set_weight_value(Double new_value) {
  if (new_value < 0 || new_value >= 1.0)
    return false;

  weight_value_ = new_value;
  return true;
}

} /* namespace dataweights */
} /* namespace observations */
} /* namespace niwa */
