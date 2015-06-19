/**
 * @file Factory.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 12/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "Factory.h"

#include "AgeLengthKeys/Manager.h"
#include "AgeLengthKeys/Children/Data.h"

// namespaces
namespace niwa {
namespace agelengthkeys {

/**
 * Create an instance of an age length key object
 *
 * @param object_type The type of object to create. Should be age_length_key
 * @param sub_type The sub type of the age length key
 * @return pointer to age length key object
 */
AgeLengthKeyPtr Factory::Create(const string& object_type, const string& sub_type) {
  AgeLengthKeyPtr object;

  if (object_type == PARAM_AGE_LENGTH_KEY) {
    if (sub_type == PARAM_DATA)
      object = AgeLengthKeyPtr(new Data());
  }

  if (object)
    agelengthkeys::Manager::Instance().AddObject(object);

  return object;
}

} /* namespace agelengthkeys */
} /* namespace niwa */
