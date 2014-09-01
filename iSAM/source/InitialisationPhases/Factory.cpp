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

#include "InitialisationPhases/Children/Derived.h"
#include "InitialisationPhases/Children/Iterative.h"

// Namespaces
namespace isam {
namespace initialisationphases {

/**
 * Create the instance of our object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g age_size, process)
 * @param sub_type The child type of the object to create (e.g ageing, schnute)
 * @return shared_ptr to the object we've created
 */
InitialisationPhasePtr Factory::Create(const string& object_type, const string& sub_type) {
  InitialisationPhasePtr result;

  if (object_type == PARAM_INITIALISATION_PHASE) {
    if (sub_type == PARAM_DERIVED)
      result = InitialisationPhasePtr(new Derived());
    else if (sub_type == "" || sub_type == PARAM_ITERATIVE)
      result = InitialisationPhasePtr(new Iterative());

    if (result)
      isam::initialisationphases::Manager::Instance().AddObject(result);
  }

  return result;
}

}
}


