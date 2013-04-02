/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 6/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Factory.h"

#include "Observations/Manager.h"
#include "Observations/Children/Abundance.h"

// Namespaces
namespace isam {
namespace observations {

/**
 * Create an observation, add it to the manager and return it.
 *
 * @param block_type The type of @block in the configuration file
 * @param object_type The value stored in the 'type' parameter for this block
 * @return shared_ptr to the observation
 */
ObservationPtr Factory::Create(const string& block_type, const string& object_type) {
  ObservationPtr result;

  if (block_type == PARAM_OBSERVATION) {
    if (object_type == PARAM_ABUNDANCE)
      result = ObservationPtr(new Abundance());
  }

  if (result)
    isam::observations::Manager::Instance().AddObject(result);

  return result;
}

} /* namespace observations */
} /* namespace isam */
