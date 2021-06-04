/**
 * @file ProcessAbundance.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 17/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
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
ProcessAbundance::ProcessAbundance(shared_ptr<Model> model) : observations::age::Abundance(model) {
  parameters_.Bind<string>(PARAM_PROCESS, &process_label_, "The label of the process for the observation", "");
  parameters_.Bind<Double>(PARAM_PROCESS_PROPORTION, &process_proportion_, "The proportion through the process when the observation is evaluated", "", Double(0.5))
      ->set_range(0.0, 1.0);

  mean_proportion_method_ = false;
}

/**
 * Build
 */
void ProcessAbundance::DoBuild() {
  Abundance::DoBuild();

  if (process_proportion_ < 0.0 || process_proportion_ > 1.0)
    LOG_ERROR_P(PARAM_PROCESS_PROPORTION) << ": process_proportion (" << process_proportion_ << ") must be between 0.0 and 1.0 inclusive";
  proportion_of_time_ = process_proportion_;

  auto time_step = model_->managers()->time_step()->GetTimeStep(time_step_label_);
  if (!time_step) {
    LOG_FATAL_P(PARAM_TIME_STEP) << "Time step label " << time_step_label_ << " was not found.";
  } else {
    for (unsigned year : years_) time_step->SubscribeToProcess(this, year, process_label_);
  }
  for (auto year : years_) {
    if ((year < model_->start_year()) || (year > model_->final_year()))
      LOG_ERROR_P(PARAM_YEARS) << "Years cannot be less than start_year (" << model_->start_year() << "), or greater than final_year (" << model_->final_year() << ").";
  }
}

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */
