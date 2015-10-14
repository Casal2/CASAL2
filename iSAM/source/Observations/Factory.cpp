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

#include "Model/Model.h"
#include "Model/Managers.h"
#include "Observations/Manager.h"
#include "Observations/Children/ProcessAbundance.h"
#include "Observations/Children/ProcessBiomass.h"
#include "Observations/Children/ProcessProportionsAtAge.h"
#include "Observations/Children/ProcessProportionsAtAgeForFishery.h"
#include "Observations/Children/ProcessProportionsAtLength.h"
#include "Observations/Children/ProcessProportionsAtLengthForFishery.h"
#include "Observations/Children/ProcessProportionsByCategory.h"
#include "Observations/Children/TimeStepAbundance.h"
#include "Observations/Children/TimeStepBiomass.h"
#include "Observations/Children/TimeStepProportionsAtAge.h"
#include "Observations/Children/TimeStepProportionsAtLength.h"
#include "Observations/Children/TimeStepProportionsByCategory.h"

// Namespaces
namespace niwa {
namespace observations {

/**
 * Create the instance of our object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g age_size, process)
 * @param sub_type The child type of the object to create (e.g ageing, schnute)
 * @return shared_ptr to the object we've created
 */
Observation* Factory::Create(Model* model, const string& object_type, const string& sub_type) {
  Observation* result = nullptr;

  if (object_type == PARAM_OBSERVATION) {
    if (sub_type == PARAM_ABUNDANCE)
      result = new TimeStepAbundance(model);
    else if (sub_type == PARAM_PROCESS_ABUNDANCE)
      result = new ProcessAbundance(model);
    else if (sub_type == PARAM_BIOMASS)
      result = new TimeStepBiomass(model);
    else if (sub_type == PARAM_PROCESS_BIOMASS)
      result = new ProcessBiomass(model);
    else if (sub_type == PARAM_PROPORTIONS_AT_AGE)
      result = new TimeStepProportionsAtAge(model);
    else if (sub_type == PARAM_PROPORTIONS_AT_LENGTH)
      result = new TimeStepProportionsAtLength(model);
    else if (sub_type == PARAM_PROCESS_PROPORTIONS_AT_AGE)
      result = new ProcessProportionsAtAge(model);
    else if (sub_type == PARAM_PROCESS_PROPORTIONS_AT_AGE_FOR_FISHERY)
      result = new ProcessProportionsAtAgeForFishery(model);
    else if (sub_type == PARAM_PROCESS_PROPORTIONS_AT_LENGTH)
      result = new ProcessProportionsAtLength(model);
    else if (sub_type == PARAM_PROCESS_PROPORTIONS_AT_LENGTH_FOR_FISHERY)
      result = new ProcessProportionsAtLengthForFishery(model);
    else if (sub_type == PARAM_PROCESS_PROPORTIONS_BY_CATEGORY)
      result = new ProcessProportionsByCategory(model);
    else if (sub_type == PARAM_PROPORTIONS_BY_CATEGORY)
      result = new TimeStepProportionsByCategory(model);
  }

  if (result)
    model->managers().observation()->AddObject(result);

  return result;
}

} /* namespace observations */
} /* namespace niwa */
