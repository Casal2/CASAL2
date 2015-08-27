/**
 * @file ProcessProportionsAtAgeForFishery.cpp
 * @author C Marsh
 * @github https://github.com/Zaita
 * @date 25/08/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "ProcessProportionsAtAgeForFishery.h"

#include <memory>

#include "TimeSteps/Manager.h"
#include "Utilities/DoubleCompare.h"

// namespaces
namespace niwa {
namespace observations {

/**
 *
 */
ProcessProportionsAtAgeForFishery::ProcessProportionsAtAgeForFishery() {
  parameters_.Bind<string>(PARAM_PROCESS, &process_label_, "Process label", "");
  parameters_.Bind<Double>(PARAM_PROCESS_PROPORTION, &process_proportion_, "Process proportion", "", Double(0.5));

  mean_proportion_method_ = false;
}

/**
 *
 */
void ProcessProportionsAtAgeForFishery::DoBuild() {
  ProportionsAtAgeForFishery::DoBuild();

  if (process_proportion_ < 0.0 || process_proportion_ > 1.0)
    LOG_ERROR_P(PARAM_PROCESS_PROPORTION) << ": process_proportion (" << AS_DOUBLE(process_proportion_) << ") must be between 0.0 and 1.0";
  proportion_of_time_ = process_proportion_;

  TimeStepPtr time_step = timesteps::Manager::Instance().GetTimeStep(time_step_label_);
  if (!time_step)
    LOG_FATAL_P(PARAM_TIME_STEP) << time_step_label_ << " could not be found. Have you defined it?";
  else {
    ProcessPtr process = time_step->SubscribeToProcess(shared_ptr(), years_, process_label_);
    mortality_instantaneous_ = std::dynamic_pointer_cast<processes::MortalityInstantaneous>(process);
  }

  if (!mortality_instantaneous_)
    LOG_ERROR_P(PARAM_PROCESS) << "This observation can only be used for Process of type = " << PARAM_MORTALITY_INSTANTANEOUS;
}

} /* namespace observations */
} /* namespace niwa */
