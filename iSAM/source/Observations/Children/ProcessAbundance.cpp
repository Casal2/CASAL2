/**
 * @file ProcessAbundance.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 17/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "ProcessAbundance.h"

#include "TimeSteps/Manager.h"

// namespaces
namespace niwa {
namespace observations {

/**
 *
 */
ProcessAbundance::ProcessAbundance(Model* model)
  : observations::Abundance(model) {

  parameters_.Bind<string>(PARAM_CATCHABILITY, &catchability_label_, "TBA", "");
  parameters_.Bind<string>(PARAM_OBS, &obs_, "Observation values", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years to execute in", "");
  parameters_.Bind<Double>(PARAM_ERROR_VALUE, &error_values_, "The error values to use against the observation values", "");
  parameters_.Bind<Double>(PARAM_DELTA, &delta_, "Delta value for error values", "", Double(1e-10));
  parameters_.Bind<Double>(PARAM_PROCESS_ERROR, &process_error_value_, "Process error", "", Double(0.0));
  parameters_.Bind<string>(PARAM_PROCESS, &process_label_, "Process label", "");
  parameters_.Bind<Double>(PARAM_PROCESS_PROPORTION, &process_proportion_, "Process proportion", "", Double(0.5));

  mean_proportion_method_ = false;
}

/**
 *
 */
void ProcessAbundance::DoBuild() {
  Abundance::DoBuild();

  if (process_proportion_ < 0.0 || process_proportion_ > 1.0)
    LOG_ERROR_P(PARAM_PROCESS_PROPORTION) << ": process_proportion (" << AS_DOUBLE(process_proportion_) << ") must be between 0.0 and 1.0";
  proportion_of_time_ = process_proportion_;

  auto time_step = model_->managers().time_step()->GetTimeStep(time_step_label_);
  if (!time_step)
    LOG_FATAL_P(PARAM_TIME_STEP) << time_step_label_ << " could not be found. Have you defined it?";
  else {
    for (unsigned year : years_)
      time_step->SubscribeToProcess(this, year, process_label_);
  }
}
} /* namespace observations */
} /* namespace niwa */
