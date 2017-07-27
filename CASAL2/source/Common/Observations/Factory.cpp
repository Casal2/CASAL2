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

#include "Common/Model/Model.h"
#include "Common/Model/Managers.h"
#include "Common/Observations/Manager.h"
#include "Common/Observations/Children/ProcessRemovalsByAge.h"
#include "Common/Observations/Children/ProcessRemovalsByLength.h"
#include "Common/Observations/Children/ProportionsMatureByAge.h"
#include "Common/Observations/Children/ProportionsMigrating.h"
#include "Common/Observations/Children/TagRecaptureByAge.h"
#include "Common/Observations/Children/TagRecaptureByLength.h"
#include "Common/Observations/Children/Abundance/Process.h"
#include "Common/Observations/Children/Abundance/TimeStep.h"
#include "Common/Observations/Children/Biomass/Process.h"
#include "Common/Observations/Children/Biomass/TimeStep.h"
#include "Common/Observations/Children/ProportionsAtAge/Process.h"
#include "Common/Observations/Children/ProportionsAtAge/TimeStep.h"
#include "Common/Observations/Children/ProportionsAtLength/Process.h"
#include "Common/Observations/Children/ProportionsAtLength/TimeStep.h"
#include "Common/Observations/Children/ProportionsByCategory/Process.h"
#include "Common/Observations/Children/ProportionsByCategory/TimeStep.h"

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
    else if (sub_type == PARAM_PROPORTIONS_MATURE_BY_AGE)
      result = new ProportionsMatureByAge(model);
    else if (sub_type == PARAM_PROPORTIONS_AT_LENGTH)
      result = new TimeStepProportionsAtLength(model);
    else if (sub_type == PARAM_PROCESS_PROPORTIONS_AT_AGE)
      result = new ProcessProportionsAtAge(model);
    else if (sub_type == PARAM_PROCESS_REMOVALS_BY_AGE)
      result = new ProcessRemovalsByAge(model);
    else if (sub_type == PARAM_PROCESS_PROPORTIONS_AT_LENGTH)
      result = new ProcessProportionsAtLength(model);
    else if (sub_type == PARAM_PROCESS_REMOVALS_BY_LENGTH)
      result = new ProcessRemovalsByLength(model);
    else if (sub_type == PARAM_PROCESS_PROPORTIONS_MIGRATING)
      result = new ProportionsMigrating(model);
    else if (sub_type == PARAM_PROCESS_PROPORTIONS_BY_CATEGORY)
      result = new ProcessProportionsByCategory(model);
    else if (sub_type == PARAM_PROPORTIONS_BY_CATEGORY)
      result = new TimeStepProportionsByCategory(model);
    else if (sub_type == PARAM_TAG_RECAPTURE_BY_AGE)
      result = new TagRecaptureByAge(model);
    else if (sub_type == PARAM_TAG_RECAPTURE_BY_LENGTH)
      result = new TagRecaptureByLength(model);
  }

  if (result)
    model->managers().observation()->AddObject(result);

  return result;
}

} /* namespace observations */
} /* namespace niwa */
