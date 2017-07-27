/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Factory.h"

#include "Common/Model/Model.h"
#include "Common/Model/Managers.h"
#include "Common/InitialisationPhases/Children/Cinitial.h"
#include "Common/InitialisationPhases/Children/Derived.h"
#include "Common/InitialisationPhases/Children/Iterative.h"
#include "Common/InitialisationPhases/Children/StateCategoryByAge.h"

// Namespaces
namespace niwa {
namespace initialisationphases {

/**
 * Create the instance of our object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g age_size, process)
 * @param sub_type The child type of the object to create (e.g ageing, schnute)
 * @return shared_ptr to the object we've created
 */
InitialisationPhase* Factory::Create(Model* model, const string& object_type, const string& sub_type) {
  InitialisationPhase* result = nullptr;

  if (object_type == PARAM_INITIALISATION_PHASE) {
    if (sub_type == PARAM_DERIVED)
      result = new Derived(model);
    else if (sub_type == "" || sub_type == PARAM_ITERATIVE)
      result = new Iterative(model);
    else if (sub_type == PARAM_STATE_CATEGORY_BY_AGE)
      result = new StateCategoryByAge(model);
    else if (sub_type == PARAM_CINITIAL)
      result = new Cinitial(model);

    if (result)
      model->managers().initialisation_phase()->AddObject(result);
  }

  return result;
}

}
}


