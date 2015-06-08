/**
 * @file Dispersion.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 5/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "Dispersion.h"

// namespaces
namespace niwa {
namespace observations {
namespace dataweights {

/**
 *
 */
Double Dispersion::AdjustLikelihood(Double likelihood) {
  return likelihood * (1/weight_value_);
}

/**
 *
 */
bool Dispersion::set_weight_value(Double new_value) {
  if (new_value <= 1.0)
    return false;

  weight_value_ = new_value;
  return true;
}

} /* namespace dataweights */
} /* namespace observations */
} /* namespace niwa */
