/**
 * @file Factory.cpp
 * @author  C.Marsh
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 */

// headers
#include "Factory.h"

#include "../GrowthIncrements/Length/None.h"
#include "../GrowthIncrements/Length/Exponential.h"
#include "../GrowthIncrements/Length/Basic.h"
#include "../GrowthIncrements/Manager.h"
#include "../Model/Managers.h"
#include "../Model/Model.h"

// namespaces
namespace niwa {
namespace growthincrements {

/**
 * Create the instance of the object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g., age_size, process)
 * @param sub_type The child type of the object to create (e.g., ageing, schnute)
 * @return shared_ptr to the object
 */
GrowthIncrement* Factory::Create(shared_ptr<Model> model, const string& object_type, const string& sub_type) {
  GrowthIncrement* result = nullptr;

  if (object_type == PARAM_GROWTH_INCREMENT || object_type == PARAM_GROWTH_INCREMENTS) {
    if (sub_type == PARAM_NONE)
      result = new None(model);
    else if (sub_type == PARAM_EXPONENTIAL)
      result = new Exponential(model);
    else if (sub_type == PARAM_BASIC)
      result = new Basic(model);
    if (result)
      model->managers()->growth_increment()->AddObject(result);
  }

  return result;
}

} /* namespace growthincrements */
} /* namespace niwa */
