/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 8/08/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/�2014 - www.niwa.co.nz
 *
 */

// headers
#include "Factory.h"

#include "../Model/Managers.h"
#include "../Model/Model.h"
#include "../Simulates/Common/Constant.h"
#include "../Simulates/Manager.h"

// namespaces
namespace niwa {
namespace simulates {

/**
 * Create the instance of the object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g., age_size, process)
 * @param sub_type The child type of the object to create (e.g., ageing, schnute)
 * @return shared_ptr to the object
 */
Simulate* Factory::Create(shared_ptr<Model> model, const string& object_type, const string& sub_type) {
  Simulate* result = nullptr;

  if (object_type == PARAM_PROJECTS || object_type == PARAM_PROJECT) {
    if (sub_type == PARAM_CONSTANT)
      result = new Constant(model);

    if (result)
      model->managers()->simulate()->AddObject(result);
  }

  return result;
}

} /* namespace simulates */
} /* namespace niwa */
