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
#include "Observations/Children/TimeStepAbundance.h"
#include "Observations/Children/TimeStepBiomass.h"
#include "Observations/Children/TimeStepProportionsAtAge.h"

// Namespaces
namespace isam {
namespace observations {

/**
 * Create the instance of our object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g age_size, process)
 * @param sub_type The child type of the object to create (e.g ageing, schnute)
 * @return shared_ptr to the object we've created
 */
ObservationPtr Factory::Create(const string& object_type, const string& sub_type) {
  ObservationPtr result;

  if (object_type == PARAM_OBSERVATION) {
    if (sub_type == PARAM_ABUNDANCE)
      result = ObservationPtr(new TimeStepAbundance());
    else if (sub_type == PARAM_BIOMASS)
      result = ObservationPtr(new TimeStepBiomass());
    else if (sub_type == PARAM_PROPORTIONS_AT_AGE)
      result = ObservationPtr(new TimeStepProportionsAtAge());
  }

  if (result)
    isam::observations::Manager::Instance().AddObject(result);

  return result;
}

} /* namespace observations */
} /* namespace isam */
