/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 6/03/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Factory.h"

#include "Model/Managers.h"
#include "Model/Model.h"
#include "Observations/Age/Abundance/Process.h"
#include "Observations/Age/Abundance/TimeStep.h"
#include "Observations/Age/AgeLength.h"
#include "Observations/Age/Biomass/Process.h"
#include "Observations/Age/Biomass/TimeStep.h"
#include "Observations/Age/ProcessRemovalsByAge.h"
#include "Observations/Age/ProcessRemovalsByAgeRetained.h"
#include "Observations/Age/ProcessRemovalsByAgeRetainedTotal.h"
#include "Observations/Age/ProcessRemovalsByLength.h"
#include "Observations/Age/ProcessRemovalsByLengthRetained.h"
#include "Observations/Age/ProcessRemovalsByLengthRetainedTotal.h"
#include "Observations/Age/ProcessRemovalsByWeight.h"
#include "Observations/Age/ProportionsAtAge/Process.h"
#include "Observations/Age/ProportionsAtAge/TimeStep.h"
#include "Observations/Age/ProportionsAtLength/Process.h"
#include "Observations/Age/ProportionsAtLength/TimeStep.h"
#include "Observations/Age/ProportionsByCategory/Process.h"
#include "Observations/Age/ProportionsByCategory/TimeStep.h"
#include "Observations/Age/ProportionsMatureByAge.h"
#include "Observations/Age/ProportionsMigrating.h"
#include "Observations/Age/TagRecaptureByAge.h"
#include "Observations/Age/TagRecaptureByLength.h"
#include "Observations/Age/TagRecaptureByFishery.h"
#include "Observations/Manager.h"

// Length
#include "Observations/Length/Abundance/Process.h"
#include "Observations/Length/Abundance/TimeStep.h"
#include "Observations/Length/Biomass/Process.h"
#include "Observations/Length/Biomass/TimeStep.h"
#include "Observations/Length/ProcessRemovalsByLength.h"
#include "Observations/Length/ProportionsAtLength/Process.h"
#include "Observations/Length/ProportionsAtLength/TimeStep.h"
#include "Observations/Length/TagRecaptureByLength.h"
#include "Observations/Length/TagRecaptureByLengthForGrowth.h"
#include "Observations/Length/ProportionsCategoryByLength.h"

// Namespaces
namespace niwa {
namespace observations {

/**
 * Create the instance of the object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g., age_size, process)
 * @param sub_type The child type of the object to create (e.g., ageing, schnute)
 * @return shared_ptr to the object
 */
Observation* Factory::Create(shared_ptr<Model> model, const string& object_type, const string& sub_type) {
  Observation* result = nullptr;

  if (model->partition_type() == PartitionType::kAge) {
    if (object_type == PARAM_OBSERVATION) {
      if (sub_type == PARAM_ABUNDANCE)
        result = new age::TimeStepAbundance(model);
      if (sub_type == PARAM_AGE_LENGTH)
        result = new age::AgeLength(model);
      else if (sub_type == PARAM_PROCESS_ABUNDANCE)
        result = new age::ProcessAbundance(model);
      else if (sub_type == PARAM_BIOMASS)
        result = new age::TimeStepBiomass(model);
      else if (sub_type == PARAM_PROCESS_BIOMASS)
        result = new age::ProcessBiomass(model);
      else if (sub_type == PARAM_PROPORTIONS_AT_AGE)
        result = new age::TimeStepProportionsAtAge(model);
      else if (sub_type == PARAM_PROPORTIONS_MATURE_BY_AGE)
        result = new age::ProportionsMatureByAge(model);
      else if (sub_type == PARAM_PROPORTIONS_AT_LENGTH)
        result = new age::TimeStepProportionsAtLength(model);
      else if (sub_type == PARAM_PROCESS_PROPORTIONS_AT_AGE)
        result = new age::ProcessProportionsAtAge(model);
      else if (sub_type == PARAM_PROCESS_REMOVALS_BY_AGE)
        result = new age::ProcessRemovalsByAge(model);
      else if (sub_type == PARAM_PROCESS_REMOVALS_BY_AGE_RETAINED)
        result = new age::ProcessRemovalsByAgeRetained(model);
      else if (sub_type == PARAM_PROCESS_REMOVALS_BY_AGE_RETAINED_TOTAL)
        result = new age::ProcessRemovalsByAgeRetainedTotal(model);
      else if (sub_type == PARAM_PROCESS_PROPORTIONS_AT_LENGTH)
        result = new age::ProcessProportionsAtLength(model);
      else if (sub_type == PARAM_PROCESS_REMOVALS_BY_LENGTH)
        result = new age::ProcessRemovalsByLength(model);
      else if (sub_type == PARAM_PROCESS_REMOVALS_BY_LENGTH_RETAINED)
        result = new age::ProcessRemovalsByLengthRetained(model);
      else if (sub_type == PARAM_PROCESS_REMOVALS_BY_LENGTH_RETAINED_TOTAL)
        result = new age::ProcessRemovalsByLengthRetainedTotal(model);
      else if (sub_type == PARAM_PROCESS_REMOVALS_BY_WEIGHT)
        result = new age::ProcessRemovalsByWeight(model);
      else if (sub_type == PARAM_PROCESS_PROPORTIONS_MIGRATING)
        result = new age::ProportionsMigrating(model);
      else if (sub_type == PARAM_PROCESS_PROPORTIONS_BY_CATEGORY)
        result = new age::ProcessProportionsByCategory(model);
      else if (sub_type == PARAM_PROPORTIONS_BY_CATEGORY)
        result = new age::TimeStepProportionsByCategory(model);
      else if (sub_type == PARAM_TAG_RECAPTURE_BY_FISHERY)
        result = new age::TagRecaptureByFishery(model);
      else if (sub_type == PARAM_TAG_RECAPTURE_BY_AGE)
        result = new age::TagRecaptureByAge(model);
      else if (sub_type == PARAM_TAG_RECAPTURE_BY_LENGTH)
        result = new age::TagRecaptureByLength(model);
    }
  } else if (model->partition_type() == PartitionType::kLength) {
      if (sub_type == PARAM_ABUNDANCE)
        result = new length::TimeStepAbundance(model);
      else if (sub_type == PARAM_PROCESS_ABUNDANCE)
        result = new length::ProcessAbundance(model);
      else if (sub_type == PARAM_BIOMASS)
        result = new length::TimeStepBiomass(model);
      else if (sub_type == PARAM_PROCESS_BIOMASS)
        result = new length::ProcessBiomass(model);
      else if (sub_type == PARAM_PROCESS_REMOVALS_BY_LENGTH)
        result = new length::ProcessRemovalsByLength(model);
      else if (sub_type == PARAM_PROPORTIONS_AT_LENGTH)
        result = new length::TimeStepProportionsAtLength(model);
      else if (sub_type == PARAM_TAG_RECAPTURE_BY_LENGTH)
        result = new length::TagRecaptureByLength(model);
      else if (sub_type == PARAM_TAG_RECAPTURE_BY_LENGTH_FOR_GROWTH)
        result = new length::TagRecaptureByLengthForGrowth(model);
      else if (sub_type == PARAM_PROPORTIONS_BY_CATEGORY)
        result = new length::ProportionsCategoryByLength(model);
  }

  if (result)
    model->managers()->observation()->AddObject(result);

  return result;
}

} /* namespace observations */
} /* namespace niwa */
