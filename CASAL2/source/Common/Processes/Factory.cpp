/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 13/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Factory.h"

#include "Common/Model/Model.h"
#include "Common/Model/Managers.h"
#include "Common/Processes/Manager.h"
#include "Age/Processes/Factory.h"
#include "Length/Processes/Factory.h"

// Namespaces
namespace niwa {
namespace processes {

/**
 * Create the instance of our object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g age_size, process)
 * @param sub_type The child type of the object to create (e.g ageing, schnute)
 * @return shared_ptr to the object we've created
 */
Process* Factory::Create(Model* model, const string& object_type, const string& sub_type) {
  Process* result = nullptr;

  string object = object_type;
  string sub    = sub_type;

  /**
   * If object_type is not "process" or "processes" then we're using a special
   * declaration (e.g @maturation) and we want to modify this back to the standard
   * method so we only need 1 set of conditional statements.
   */
  if (object != PARAM_PROCESS && object != PARAM_PROCESSES) {
    LOG_FINE() << "Changing object_type (" << object << ") and sub_type (" << ") to the standard declaration format";
    if (sub != "")
      sub = object_type + "_" + sub_type;
    else
      sub = object_type;

    object = PARAM_PROCESS;

    LOG_FINE() << "Finished modification of object_type (" << object << ") and sub_type (" << sub << ")";
  }

  if (model->partition_structure() == PartitionStructure::kAge) {
    result = age::processes::Factory::Create(model, object_type, sub_type);
  } else if (model->partition_structure() == PartitionStructure::kLength) {
    result = length::processes::Factory::Create(model, object_type, sub_type);
  }

  if (result)
    model->managers().process()->AddObject(result);

  //result->parameters().Get(PARAM_TYPE)->set_value(sub);
  return result;
}

} /* namespace processes */
} /* namespace niwa */
