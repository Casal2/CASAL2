/**
 * @file ProcessProportionsAtLength.cpp
 * @author C Marsh
 * @github https://github.com/Craig44
 * @date 2020
 * @section LICENSE
 *
 * Copyright NIWA Science 2020 - www.niwa.co.nz
 *
 */

// headers
#include "Process.h"

#include "TimeSteps/Manager.h"

// namespaces
namespace niwa {
namespace observations {
namespace length {

/**
 * Default constructor
 */
ProcessProportionsAtLength::ProcessProportionsAtLength(shared_ptr<Model> model) : observations::length::ProportionsAtLength(model) {
  parameters_.Bind<string>(PARAM_PROCESS, &process_label_, "The label of the process for the observation", "");
  parameters_.Bind<Double>(PARAM_PROCESS_PROPORTION, &process_proportion_, "The proportion through the process when the observation is evaluated", "", Double(0.5))
      ->set_range(0.0, 1.0);

  mean_proportion_method_ = false;
}

/**
 * Build
 */
void ProcessProportionsAtLength::DoBuild() {
  length::ProportionsAtLength::DoBuild();
  proportion_of_time_ = process_proportion_;

  TimeStep* time_step = model_->managers()->time_step()->GetTimeStep(time_step_label_);
  if (!time_step) {
    LOG_FATAL_P(PARAM_TIME_STEP) << "Time step label " << time_step_label_ << " was not found.";
  } else {
    for (unsigned year : years_) time_step->SubscribeToProcess(this, year, process_label_);
  }
}

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */
