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

#include "Asserts/Manager.h"
#include "Asserts/Children/Estimable.h"
#include "Asserts/Children/ObjectiveFunction.h"

// Namespaces
namespace isam {
namespace asserts {

/**
 * Create the instance of our object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g age_size, process)
 * @param sub_type The child type of the object to create (e.g ageing, schnute)
 * @return shared_ptr to the object we've created
 */
AssertPtr Factory::Create(string object_type, string sub_type) {
  AssertPtr result;

  if (object_type == PARAM_ASSERT) {
    if (sub_type == PARAM_ESTIMABLE)
      result = AssertPtr(new Estimable());
    else if (sub_type == PARAM_OBJECTIVE_FUNCTION)
      result = AssertPtr(new ObjectiveFunction());

    if (result)
      asserts::Manager::Instance().AddObject(result);
  }

  return result;
}

} /* namespace processes */
} /* namespace isam */
