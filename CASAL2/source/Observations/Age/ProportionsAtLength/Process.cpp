/**
 * @file ProcessProportionsAtLength.cpp
 * @author C Marsh
 * @github https://github.com/Zaita
 * @date 14/08/15
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "Process.h"

#include "TimeSteps/Manager.h"

// namespaces
namespace niwa {
namespace observations {
namespace age {

/**
 * Default constructor
 */
ProcessProportionsAtLength::ProcessProportionsAtLength(shared_ptr<Model> model) : observations::age::ProportionsAtLength(model) {
  parameters_.Bind<string>(PARAM_PROCESS, &process_label_, "The label of the process for the observation", "");
  parameters_.Bind<Double>(PARAM_PROCESS_PROPORTION, &process_proportion_, "The proportion through the process when the observation is evaluated", "", Double(0.5))
      ->set_range(0.0, 1.0);

  mean_proportion_method_ = false;
}

/**
 * Build
 */
void ProcessProportionsAtLength::DoBuild() {
  ProportionsAtLength::DoBuild();

  if (process_proportion_ < 0.0 || process_proportion_ > 1.0)
    LOG_ERROR_P(PARAM_PROCESS_PROPORTION) << ": process_proportion (" << process_proportion_ << ") must be between 0.0 and 1.0 inclusive";
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
