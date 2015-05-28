/**
 * @file Factory.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 22/04/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "Factory.h"

#include "Observations/ProcessErrors/Children/AdditiveNormal.h"
#include "Observations/ProcessErrors/Children/None.h"

// namespaces
namespace niwa {
namespace observations {
namespace processerrors {

/**
 * Create method
 */
ProcessErrorPtr Factory::Create(const std::string& type) {
  ProcessErrorPtr result;

  if (type == PARAM_ADDITIVE_NORMAL)
    result = ProcessErrorPtr(new AdditiveNormal());
  else if (type == PARAM_NONE)
    result = ProcessErrorPtr(new None());

  return result;
}

} /* namespace processerrors */
} /* namespace observations */
} /* namespace niwa */
