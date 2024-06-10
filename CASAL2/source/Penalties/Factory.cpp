/**
 * @file Factory.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 28/10/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/�2014 - www.niwa.co.nz
 *
 */

// Headers
#include "Factory.h"

#include "../Model/Managers.h"
#include "../Model/Model.h"
#include "../Penalties/Common/Process.h"
#include "../Penalties/Manager.h"

// Namespaces
namespace niwa {
namespace penalties {

/**
 * Create the instance of the object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g., age_size, process)
 * @param sub_type The child type of the object to create (e.g., ageing, schnute)
 * @return shared_ptr to the object
 */
Penalty* Factory::Create(shared_ptr<Model> model, const string& object_type, const string& sub_type) {
  Penalty* object = nullptr;

  if (object_type == PARAM_PENALTY) {
    if (sub_type == PARAM_PROCESS)
      object = new Process(model);

    if (object)
      model->managers()->penalty()->AddObject(object);
  }

  return object;
}

} /* namespace penalties */
} /* namespace niwa */
