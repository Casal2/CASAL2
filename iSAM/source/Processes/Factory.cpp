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
#include "Processes/Children/MaturationRate.h"
#include "Processes/Children/MortalityConstantRate.h"
#include "Processes/Children/MortalityEvent.h"
#include "Processes/Children/RecruitmentBevertonHolt.h"
#include "Processes/Children/RecruitmentConstant.h"

// Namespaces
namespace isam {
namespace processes {

/**
 *
 */
ProcessPtr Factory::Create(const string& block_type, const string& process_type) {

  ProcessPtr result;

  if (block_type == PARAM_AGEING && process_type == "") {
    result = ProcessPtr(new Ageing());

  } else if (block_type == PARAM_RECRUITMENT && process_type == PARAM_BEVERTON_HOLT){
    result = ProcessPtr(new RecruitmentBevertonHolt());

  } else if (block_type == PARAM_RECRUITMENT && process_type == PARAM_CONSTANT) {
    result = ProcessPtr(new RecruitmentConstant());

  } else if (block_type == PARAM_MATURATION && process_type == PARAM_RATE) {
    result = ProcessPtr(new MaturationRate());

  } else if (block_type == PARAM_MORTALITY && process_type == PARAM_CONSTANT_RATE) {
    result = ProcessPtr(new MortalityConstantRate());

  } else if (block_type == PARAM_MORTALITY && process_type == PARAM_EVENT) {
    result = ProcessPtr(new MortalityEvent());
  }

  if (result)
    isam::processes::Manager::Instance().AddObject(result);

  return result;
}

} /* namespace processes */
} /* namespace isam */
