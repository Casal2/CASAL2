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
#include "Processes/Children/Ageing.h"
#include "Processes/Children/Maturation.h"
#include "Processes/Children/MortalityConstantRate.h"
#include "Processes/Children/MortalityEvent.h"
#include "Processes/Children/MortalityEventBiomass.h"
#include "Processes/Children/MortalityInstantaneous.h"
#include <Processes/Children/MortalityInitialisationEvent.h>
#include <Processes/Children/MortalityInitialisationEventBiomass.h>
#include "Processes/Children/MortalityPreySuitability.h"
#include "Processes/Children/MortalityHollingRate.h"
#include "Processes/Children/Nop.h"
#include "Processes/Children/RecruitmentBevertonHolt.h"
#include "Processes/Children/RecruitmentBevertonHoltWithDeviations.h"
#include "Processes/Children/RecruitmentConstant.h"
#include "Processes/Children/TagByAge.h"
#include "Processes/Children/TagByLength.h"
#include "Processes/Children/TagLoss.h"
#include "Processes/Children/TransitionCategory.h"
#include "Processes/Children/TransitionCategoryByAge.h"

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

  if (object == PARAM_PROCESS || object == PARAM_PROCESSES) {
    if (sub == PARAM_AGEING)
      result = new Ageing(model);
    else if (sub == PARAM_RECRUITMENT_BEVERTON_HOLT)
      result = new RecruitmentBevertonHolt(model);
    else if (sub == PARAM_RECRUITMENT_BEVERTON_HOLT_WITH_DEVIATIONS)
      result = new RecruitmentBevertonHoltWithDeviations(model);
    else if (sub == PARAM_RECRUITMENT_CONSTANT)
      result = new RecruitmentConstant(model);
    else if (sub == PARAM_MATURATION)
      result = new Maturation(model);
    else if (sub == PARAM_MORTALITY_CONSTANT_RATE)
      result = new MortalityConstantRate(model);
    else if (sub == PARAM_MORTALITY_INITIALISATION_EVENT)
      result = new MortalityInitialisationEvent(model);
    else if (sub == PARAM_MORTALITY_INITIALISATION_EVENT_BIOMSS)
      result = new MortalityInitialisationEventBiomass(model);
    else if (sub == PARAM_MORTALITY_EVENT)
      result = new MortalityEvent(model);
    else if (sub == PARAM_MORTALITY_EVENT_BIOMASS)
      result = new MortalityEventBiomass(model);
    else if (sub == PARAM_MORTALITY_INSTANTANEOUS)
      result = new MortalityInstantaneous(model);
    else if (sub == PARAM_MORTALITY_HOLLING_RATE)
      result = new MortalityHollingRate(model);
    else if (sub == PARAM_PREY_SUITABILITY_PREDATION)
      result = new MortalityPreySuitability(model);
    else if (sub == PARAM_NOP)
      result = new Nop(model);
    else if (sub == PARAM_TAG_BY_AGE)
      result = new TagByAge(model);
    else if (sub == PARAM_TAG_BY_LENGTH)
      result = new TagByLength(model);
    else if (sub == PARAM_TAG_LOSS)
      result = new TagLoss(model);
    else if (sub == PARAM_TRANSITION_CATEGORY)
      result = new TransitionCategory(model);
    else if (sub == PARAM_TRANSITION_CATEGORY_BY_AGE)
      result = new TransitionCategoryByAge(model);

    if (result)
      model->managers().process()->AddObject(result);
  }

  //result->parameters().Get(PARAM_TYPE)->set_value(sub);
  return result;
}

} /* namespace processes */
} /* namespace niwa */
