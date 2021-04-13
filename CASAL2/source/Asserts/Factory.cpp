/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 13/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 */

// headers
#include "Factory.h"

#include "../Model/Model.h"
#include "../Model/Managers.h"
#include "../Asserts/Manager.h"
#include "../Asserts/Common/Addressable.h"
#include "../Asserts/Common/ObjectiveFunction.h"
#include "../Asserts/Common/Partition.h"

// namespaces
namespace niwa {
namespace asserts {

/**
 * Create the instance of the model object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param model The model object
 * @param object_type The type of object to create (e.g., age_size, process)
 * @param sub_type The child type of the object to create (e.g., ageing, schnute)
 * @return shared_ptr to the object created
 */
Assert* Factory::Create(shared_ptr<Model> model, const string& object_type, const string& sub_type) {
  Assert* result = nullptr;

  if (object_type == PARAM_ASSERT) {
    if (sub_type == PARAM_ADDRESSABLE)
      result = new Addressable(model);
    else if (sub_type == PARAM_OBJECTIVE_FUNCTION)
      result = new ObjectiveFunction(model);
    else if (sub_type == PARAM_PARTITION)
      result = new Partition(model);

    if (result)
      model->managers()->assertx()->AddObject(result);
  }

  return result;
}

} /* namespace asserts */
} /* namespace niwa */
