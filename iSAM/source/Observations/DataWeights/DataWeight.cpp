/**
 * @file DataWeight.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 5/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "DataWeight.h"

// namespaces
namespace niwa {
namespace observations {

/**
 * Default error_value adjustment method
 */
Double DataWeight::AdjustErrorValue(Double error_value, Double process_error) {
  if (error_value > 0.0 && process_error > 0.0)
    return (1.0 /  (1.0 / error_value + 1.0 / process_error));

  return error_value;
}

} /* namespace observations */
} /* namespace niwa */
