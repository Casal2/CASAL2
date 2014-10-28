/**
 * @file Factory.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 28/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// Headers
#include "Factory.h"

#include "Penalties/Manager.h"
#include "Penalties/Children/Process.h"
#include "Penalties/Children/VectorAverage.h"
#include "Penalties/Children/VectorSmoothing.h"

// Namespaces
namespace isam {
namespace penalties {

/**
 * Create the instance of our object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g age_size, process)
 * @param sub_type The child type of the object to create (e.g ageing, schnute)
 * @return shared_ptr to the object we've created
 */
PenaltyPtr Factory::Create(string object_type, string sub_type) {
  PenaltyPtr object;

  if (object_type == PARAM_PENALTY) {
    if (sub_type == PARAM_PROCESS)
      object = PenaltyPtr(new Process());
    else if (sub_type == PARAM_VECTOR_AVERAGE)
      object = PenaltyPtr(new VectorAverage());
    else if (sub_type == PARAM_VECTOR_SMOOTHING)
      object = PenaltyPtr(new VectorSmoothing());

    if (object)
      penalties::Manager::Instance().AddObject(object);
  }

  return object;
}

} /* namespace penalties */
} /* namespace isam */
