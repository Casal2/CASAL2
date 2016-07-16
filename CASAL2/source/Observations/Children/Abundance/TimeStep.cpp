/**
 * @file TimeStepAbundance.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 17/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */

// headers
#include "TimeStep.h"

#include "TimeSteps/Manager.h"

// namespaces
namespace niwa {
namespace observations {

/**
 *
 */
TimeStepAbundance::TimeStepAbundance(Model* model)
  : observations::Abundance(model) {
  parameters_.Bind<string>(PARAM_CATCHABILITY, &catchability_label_, "Catchability label for this observation", "");
  parameters_.Bind<string>(PARAM_OBS, &obs_, "Observation values", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years to execute in", "");
  parameters_.Bind<Double>(PARAM_ERROR_VALUE, &error_values_, "The error values to use against the observation values", "");
  parameters_.Bind<Double>(PARAM_DELTA, &delta_, "Delta value for error values", "", Double(1e-10));
  parameters_.Bind<Double>(PARAM_PROCESS_ERROR, &process_error_value_, "Process error", "", Double(0.0));
  parameters_.Bind<Double>(PARAM_TIME_STEP_PROPORTION, &time_step_proportion_, "Proportion through the time step to analyse the partition from", "", Double(0.5));

  mean_proportion_method_ = true;
}

/**
 *
 */
void TimeStepAbundance::DoBuild() {
  Abundance::DoBuild();

  if (time_step_proportion_ < 0.0 || time_step_proportion_ > 1.0)
    LOG_ERROR_P(PARAM_TIME_STEP_PROPORTION) << ": time_step_proportion (" << AS_DOUBLE(time_step_proportion_) << ") must be between 0.0 and 1.0";
  proportion_of_time_ = time_step_proportion_;

  auto time_step = model_->managers().time_step()->GetTimeStep(time_step_label_);
  if (!time_step) {
    LOG_ERROR_P(PARAM_TIME_STEP) << time_step_label_ << " could not be found. Have you defined it?";
  } else {
    for (unsigned year : years_)
      time_step->SubscribeToBlock(this, year);
  }
}

} /* namespace observations */
} /* namespace niwa */
