/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 13/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 *
 */

// Headers
#include "Factory.h"

#include "Model/Model.h"
#include "Model/Managers.h"
#include "InitialisationPhases/Manager.h"
#include "InitialisationPhases/Age/Cinitial.h"
#include "InitialisationPhases/Age/Derived.h"
#include "InitialisationPhases/Age/Iterative.h"
#include "InitialisationPhases/Age/StateCategoryByAge.h"
#include "InitialisationPhases/Length/Iterative.h"

// Namespaces
namespace niwa {
namespace initialisationphases {

/**
 * Create the instance of the object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g., age_size, process)
 * @param sub_type The child type of the object to create (e.g., ageing, schnute)
 * @return shared_ptr to the object
 */
InitialisationPhase* Factory::Create(Model* model, const string& object_type, const string& sub_type) {
  InitialisationPhase* result = nullptr;

  if (model->partition_type() == PartitionType::kAge) {
    if (object_type == PARAM_INITIALISATION_PHASE) {
      if (sub_type == PARAM_DERIVED)
        result = new age::Derived(model);
      else if (sub_type == "" || sub_type == PARAM_ITERATIVE)
        result = new age::Iterative(model);
      else if (sub_type == PARAM_STATE_CATEGORY_BY_AGE)
        result = new age::StateCategoryByAge(model);
      else if (sub_type == PARAM_CINITIAL)
        result = new age::Cinitial(model);
     }
  } else if (model->partition_type() == PartitionType::kLength) {
    if (sub_type == "" || sub_type == PARAM_ITERATIVE)
          result = new length::Iterative(model);
  }

  if (result)
    model->managers().initialisation_phase()->AddObject(result);

  return result;
}

} /* namespace processes */
} /* namespace niwa */
