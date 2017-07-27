/**
 * @file Factory.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 9/10/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */

// Headers
#include "Factory.h"

#include "Common/Model/Model.h"
#include "Common/Model/Managers.h"
#include "Common/Profiles/Manager.h"

// Namespaces
namespace niwa {
namespace profiles {

/**
 * Create the instance of our object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g age_size, process)
 * @param sub_type The child type of the object to create (e.g ageing, schnute)
 * @return pointer to the object we've created
 */
Profile* Factory::Create(Model* model, const string& object_type, const string& sub_type) {
  Profile* object = nullptr;

  if (object_type == PARAM_PROFILE) {
    object = new Profile(model);

    if (object)
      model->managers().profile()->AddObject(object);
  }

  return object;
}

} /* namespace profiles */
} /* namespace niwa */
