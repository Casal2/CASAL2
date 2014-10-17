/**
 * @file TimeStepBiomass.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 17/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "TimeStepBiomass.h"

#include "TimeSteps/Manager.h"

// namespaces
namespace isam {
namespace observations {

/**
 *
 */
TimeStepBiomass::TimeStepBiomass() {
  parameters_.Bind<string>(PARAM_CATCHABILITY, &catchability_label_, "TBA", "");
  parameters_.Bind<string>(PARAM_OBS, &obs_, "Observation values", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years to execute in", "");
  parameters_.Bind<double>(PARAM_ERROR_VALUE, &error_values_, "The error values to use against the observation values", "");
  parameters_.Bind<double>(PARAM_DELTA, &delta_, "Delta value for error values", "", 1e-10);
  parameters_.Bind<double>(PARAM_PROCESS_ERROR, &process_error_, "Process error", "", 0.0);
  parameters_.Bind<double>(PARAM_TIME_STEP_PROPORTION, &time_step_proportion_, "Proportion through the time step to analyse the partition from", "", 0.5);

  mean_proportion_method_ = true;
}

/**
 *
 */
void TimeStepBiomass::DoBuild() {
  Biomass::DoBuild();

  if (time_step_proportion_ < 0.0 || time_step_proportion_ > 1.0)
    LOG_ERROR(parameters_.location(PARAM_TIME_STEP_PROPORTION) << ": time_step_proportion (" << AS_DOUBLE(time_step_proportion_) << ") must be between 0.0 and 1.0");
  proportion_of_time_ = time_step_proportion_;

  TimeStepPtr time_step = timesteps::Manager::Instance().GetTimeStep(time_step_label_);
  if (!time_step)
    LOG_ERROR(parameters_.location(PARAM_TIME_STEP) << time_step_label_ << " could not be found. Have you defined it?");

  for (unsigned year : years_)
    time_step->SubscribeToBlock(shared_ptr(), year);
}

} /* namespace observations */
} /* namespace isam */
