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

#include "Processes/Manager.h"
#include "Processes/Children/Ageing.h"
#include "Processes/Children/Growth.h"
#include "Processes/Children/Maturation.h"
#include "Processes/Children/MaturationRate.h"
#include "Processes/Children/MortalityConstantRate.h"
#include "Processes/Children/MortalityEvent.h"
#include "Processes/Children/MortalityEventBiomass.h"
#include "Processes/Children/RecruitmentBevertonHolt.h"
#include "Processes/Children/RecruitmentConstant.h"
#include "Processes/Children/TagByAge.h"
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
ProcessPtr Factory::Create(string object_type, string sub_type) {
  ProcessPtr result;

  /**
   * If object_type is not "process" or "processes" then we're using a special
   * declaration (e.g @maturation) and we want to modify this back to the standard
   * method so we only need 1 set of conditional statements.
   */
  if (object_type != PARAM_PROCESS && object_type != PARAM_PROCESSES) {
    LOG_FINE() << "Changing object_type (" << object_type << ") and sub_type (" << ") to the standard declaration format";
    if (sub_type != "")
      sub_type = object_type + "_" + sub_type;
    else
      sub_type = object_type;

    object_type = PARAM_PROCESS;

    LOG_FINE() << "Finished modification of object_type (" << object_type << ") and sub_type (" << sub_type << ")";
  }

  if (object_type == PARAM_PROCESS || object_type == PARAM_PROCESSES) {
    if (sub_type == PARAM_AGEING)
      result = ProcessPtr(new Ageing());
    else if (sub_type == PARAM_GROWTH)
      result = ProcessPtr(new Growth());
    else if (sub_type == PARAM_RECRUITMENT_BEVERTON_HOLT)
      result = ProcessPtr(new RecruitmentBevertonHolt());
    else if (sub_type == PARAM_RECRUITMENT_CONSTANT)
      result = ProcessPtr(new RecruitmentConstant());
    else if (sub_type == PARAM_MATURATION)
      result = ProcessPtr(new Maturation());
    else if (sub_type == PARAM_MATURATION_RATE)
      result = ProcessPtr(new MaturationRate());
    else if (sub_type == PARAM_MORTALITY_CONSTANT_RATE)
      result = ProcessPtr(new MortalityConstantRate());
    else if (sub_type == PARAM_MORTALITY_EVENT)
      result = ProcessPtr(new MortalityEvent());
    else if (sub_type == PARAM_MORTALITY_EVENT_BIOMASS)
      result = ProcessPtr(new MortalityEventBiomass());
    else if (sub_type == PARAM_TAG_BY_AGE)
      result = ProcessPtr(new TagByAge());
    else if (sub_type == PARAM_TRANSITION_CATEGORY_BY_AGE)
      result = ProcessPtr(new TransitionCategoryByAge());

    if (result)
      processes::Manager::Instance().AddObject(result);
  }

  return result;
}

} /* namespace processes */
} /* namespace niwa */
