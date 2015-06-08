/**
 * @file Multiplicative.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 5/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "Multiplicative.h"

// namespaces
namespace niwa {
namespace observations {
namespace dataweights {

/**
 *
 */
Double Multiplicative::AdjustLikelihood(Double likelihood) {
  return likelihood * weight_value_;
}

/**
 *
 */
bool Multiplicative::set_weight_value(Double new_value) {
  if (new_value <= 0.0)
    return false;

  weight_value_ = new_value;
  return true;
}

} /* namespace dataweights */
} /* namespace observations */
} /* namespace niwa */
