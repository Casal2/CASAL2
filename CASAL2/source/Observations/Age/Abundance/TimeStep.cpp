/**
 * @file TimeStepAbundance.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 17/10/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "TimeStep.h"

#include "TimeSteps/Manager.h"

// namespaces
namespace niwa {
namespace observations {
namespace age {

/**
 * Default constructor
 */
TimeStepAbundance::TimeStepAbundance(shared_ptr<Model> model) : observations::age::Abundance(model) {
  parameters_
      .Bind<Double>(PARAM_TIME_STEP_PROPORTION, &time_step_proportion_, "The proportion through the mortality block of the time step when the observation is evaluated", "",
                    Double(0.5))
      ->set_range(0.0, 1.0);

  mean_proportion_method_ = true;
}

/**
 * Build
 */
void TimeStepAbundance::DoBuild() {
  Abundance::DoBuild();

  if (time_step_proportion_ < 0.0 || time_step_proportion_ > 1.0)
    LOG_ERROR_P(PARAM_TIME_STEP_PROPORTION) << ": time_step_proportion (" << time_step_proportion_ << ") must be between 0.0 and 1.0 inclusive";
  proportion_of_time_ = time_step_proportion_;

  auto time_step = model_->managers()->time_step()->GetTimeStep(time_step_label_);
  if (!time_step) {
    LOG_ERROR_P(PARAM_TIME_STEP) << "Time step label " << time_step_label_ << " was not found.";
  } else {
    for (unsigned year : years_) time_step->SubscribeToBlock(this, year);
  }
  for (auto year : years_) {
    if ((year < model_->start_year()) || (year > model_->final_year()))
      LOG_ERROR_P(PARAM_YEARS) << "Years cannot be less than start_year (" << model_->start_year() << "), or greater than final_year (" << model_->final_year() << ").";
  }
}

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */
