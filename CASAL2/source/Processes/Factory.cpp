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

#include "Model/Model.h"
#include "Model/Managers.h"
#include "Processes/Manager.h"
#include "Processes/Age/Ageing.h"
#include "Processes/Age/Maturation.h"
#include "Processes/Age/MortalityConstantRate.h"
#include "Processes/Age/MortalityEvent.h"
#include "Processes/Age/MortalityEventBiomass.h"
#include "Processes/Age/MortalityInstantaneous.h"
#include "Processes/Age/MortalityInitialisationEvent.h"
#include "Processes/Age/MortalityInitialisationEventBiomass.h"
#include "Processes/Age/MortalityPreySuitability.h"
#include "Processes/Age/MortalityHollingRate.h"
#include "Processes/Age/RecruitmentBevertonHolt.h"
#include "Processes/Age/RecruitmentBevertonHoltWithDeviations.h"
#include "Processes/Age/RecruitmentConstant.h"
#include "Processes/Age/TagByAge.h"
#include "Processes/Age/TagByLength.h"
#include "Processes/Age/TagLoss.h"
#include "Processes/Age/TransitionCategory.h"
#include "Processes/Age/TransitionCategoryByAge.h"
#include "Processes/Common/Nop.h"
#include "Processes/Length/GrowthBasic.h"
#include "Processes/Length/MortalityConstantRate.h"
#include "Processes/Length/RecruitmentConstant.h"
#include "Processes/Age/MortalityInstantaneousRetained.h"


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
Process* Factory::Create(Model* model, const string& object_type, const string& sub_type, PartitionType partition_type) {
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

  if (model->partition_type() == PartitionType::kAge || (partition_type == PartitionType::kModel && model->partition_type() == PartitionType::kAge))
    if (object == PARAM_PROCESS || object == PARAM_PROCESSES) {
        if (sub == PARAM_AGEING)
          result = new age::Ageing(model);
        else if (sub == PARAM_RECRUITMENT_BEVERTON_HOLT)
          result = new age::RecruitmentBevertonHolt(model);
        else if (sub == PARAM_RECRUITMENT_BEVERTON_HOLT_WITH_DEVIATIONS)
          result = new age::RecruitmentBevertonHoltWithDeviations(model);
        else if (sub == PARAM_RECRUITMENT_CONSTANT)
          result = new age::RecruitmentConstant(model);
        else if (sub == PARAM_MATURATION)
          result = new age::Maturation(model);
        else if (sub == PARAM_MORTALITY_CONSTANT_RATE)
          result = new age::MortalityConstantRate(model);
        else if (sub == PARAM_MORTALITY_INITIALISATION_EVENT)
          result = new age::MortalityInitialisationEvent(model);
        else if (sub == PARAM_MORTALITY_INITIALISATION_EVENT_BIOMSS)
          result = new age::MortalityInitialisationEventBiomass(model);
        else if (sub == PARAM_MORTALITY_EVENT)
          result = new age::MortalityEvent(model);
        else if (sub == PARAM_MORTALITY_EVENT_BIOMASS)
          result = new age::MortalityEventBiomass(model);
        else if (sub == PARAM_MORTALITY_INSTANTANEOUS)
          result = new age::MortalityInstantaneous(model);
        else if (sub == PARAM_MORTALITY_INSTANTANEOUS_RETAINED)
          result = new age::MortalityInstantaneousRetained(model);
        else if (sub == PARAM_MORTALITY_HOLLING_RATE)
          result = new age::MortalityHollingRate(model);
        else if (sub == PARAM_PREY_SUITABILITY_PREDATION)
          result = new age::MortalityPreySuitability(model);
        else if (sub == PARAM_NOP)
          result = new processes::Nop(model);
        else if (sub == PARAM_TAG_BY_AGE)
          result = new age::TagByAge(model);
        else if (sub == PARAM_TAG_BY_LENGTH)
          result = new age::TagByLength(model);
        else if (sub == PARAM_TAG_LOSS)
          result = new age::TagLoss(model);
        else if (sub == PARAM_TRANSITION_CATEGORY)
          result = new age::TransitionCategory(model);
        else if (sub == PARAM_TRANSITION_CATEGORY_BY_AGE)
          result = new age::TransitionCategoryByAge(model);
      }
  if (model->partition_type() == PartitionType::kLength || (partition_type == PartitionType::kModel && model->partition_type() == PartitionType::kLength)) {
    if (object == PARAM_PROCESS || object == PARAM_PROCESSES) {
      if (sub == PARAM_GROWTH_BASIC)
        result = new length::GrowthBasic(model);
      else if (sub == PARAM_MORTALITY_CONSTANT_RATE)
        result = new length::MortalityConstantRate(model);
      else if (sub == PARAM_RECRUITMENT_CONSTANT)
        result = new length::RecruitmentConstant(model);
    }
  }

  if (result)
    model->managers().process()->AddObject(result);

  return result;
}

} /* namespace processes */
} /* namespace niwa */
