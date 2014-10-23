/**
 * @file Factory.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 24/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Factory.h"

#include "AdditionalPriors/Manager.h"
#include "AdditionalPriors/Children/Beta.h"

namespace isam {
namespace additionalpriors {

AdditionalPriorPtr Factory::Create(string object_type, string sub_type) {
  AdditionalPriorPtr object;

  if (object_type == PARAM_ADDITIONAL_PRIOR) {
    if (sub_type == PARAM_BETA)
      object = AdditionalPriorPtr(new Beta());
  }

  if (object)
    additionalpriors::Manager::Instance().AddObject(object);

  return object;
}

} /* namespace additionalpriors */
} /* namespace isam */
