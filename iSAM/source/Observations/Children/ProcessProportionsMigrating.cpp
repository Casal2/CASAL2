/**
 * @file ProcessProportionsMigrating.cpp
 * @author C. Marsh
 * @github https://github.com/Zaita
 * @date 8/10/14
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "ProcessProportionsMigrating.h"

#include <memory>

#include "TimeSteps/Manager.h"
#include "Utilities/DoubleCompare.h"

// namespaces
namespace niwa {
namespace observations {

/**
 *
 */
ProcessProportionsMigrating::ProcessProportionsMigrating() {
  parameters_.Bind<string>(PARAM_PROCESS, &process_label_, "Process label", "");
  parameters_.Bind<Double>(PARAM_PROCESS_PROPORTION, &process_proportion_, "Process proportion", "", Double(0.5));

  mean_proportion_method_ = false;
}

/**
 *
 */
void ProcessProportionsMigrating::DoBuild() {
  ProportionsMigrating::DoBuild();

  TimeStepPtr time_step = timesteps::Manager::Instance().GetTimeStep(time_step_label_);
  if (!time_step)
    LOG_FATAL_P(PARAM_TIME_STEP) << time_step_label_ << " could not be found. Have you defined it?";
  else {
    ProcessPtr process = time_step->SubscribeToProcess(shared_ptr(), years_, process_label_);
  }
}

} /* namespace observations */
} /* namespace niwa */
