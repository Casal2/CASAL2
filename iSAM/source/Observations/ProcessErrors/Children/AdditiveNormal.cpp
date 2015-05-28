/**
 * @file AdditiveNormal.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 22/04/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "AdditiveNormal.h"

#include <cmath>

// namespaces
namespace niwa {
namespace observations {
namespace processerrors {

/**
 *
 */
Double AdditiveNormal::CalculateScore(Double process_error, Double error_value, Double likelihood) {
  if (process_error == 0.0)
    return 0.0;

  return sqrt(process_error * process_error + error_value * error_value);
}

} /* namespace processerrors */
} /* namespace observations */
} /* namespace niwa */
