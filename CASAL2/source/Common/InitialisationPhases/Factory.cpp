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

#include "Common/Model/Model.h"
#include "Common/Model/Managers.h"
#include "Common/InitialisationPhases/Manager.h"
#include "Age/InitialisationPhases/Factory.h"

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

  string object = object_type;
  string sub    = sub_type;


  if (model->partition_structure() == PartitionStructure::kAge) {
    result = age::initialisationphases::Factory::Create(model, object_type, sub_type);
  }

  if (result)
    model->managers().initialisation_phase()->AddObject(result);

  return result;
}

} /* namespace processes */
} /* namespace niwa */
