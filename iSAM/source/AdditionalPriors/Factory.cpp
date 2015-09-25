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

// namespaces
namespace niwa {
namespace additionalpriors {

/**
 * Create an additional prior
 *
 * @param object_type The type of object
 * @param sub_type The sub type (e.g beta)
 * @return smart_ptr of an additional prior
 */
AdditionalPrior* Factory::Create(const string& object_type, const string& sub_type) {
  AdditionalPrior* object = nullptr;

  if (object_type == PARAM_ADDITIONAL_PRIOR) {
    if (sub_type == PARAM_BETA)
      object = new Beta();
  }

  if (object)
    additionalpriors::Manager::Instance().AddObject(object);

  return object;
}

} /* namespace additionalpriors */
} /* namespace niwa */
