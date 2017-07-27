/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 13/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 */

// headers
#include "Factory.h"

#include "Common/Model/Model.h"
#include "Common/Model/Managers.h"
#include "Common/Asserts/Manager.h"
#include "Common/Asserts/Children/Addressable.h"
#include "Common/Asserts/Children/ObjectiveFunction.h"

// namespaces
namespace niwa {
namespace asserts {

/**
 * Create the instance of our object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g age_size, process)
 * @param sub_type The child type of the object to create (e.g ageing, schnute)
 * @return shared_ptr to the object we've created
 */
Assert* Factory::Create(Model* model, const string& object_type, const string& sub_type) {
  Assert* result = nullptr;

  if (object_type == PARAM_ASSERT) {
    if (sub_type == PARAM_ADDRESSABLE)
      result = new Addressable(model);
    else if (sub_type == PARAM_OBJECTIVE_FUNCTION)
      result = new ObjectiveFunction(model);

    if (result)
      model->managers().assertx()->AddObject(result);
  }

  return result;
}

} /* namespace asserts */
} /* namespace niwa */
