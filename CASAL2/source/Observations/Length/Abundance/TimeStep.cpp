/**
 * @file TimeStepAbundance.cpp
 * @author C.Marsh
 * @github https://github.com/Craig44
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 */

// headers
#include "TimeStep.h"

#include "TimeSteps/Manager.h"

// namespaces
namespace niwa {
namespace observations {
namespace length {

/**
 * Default constructor
 */
TimeStepAbundance::TimeStepAbundance(shared_ptr<Model> model) : observations::length::Abundance(model) {
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
  length::Abundance::DoBuild();
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

} /* namespace length */
} /* namespace observations */
} /* namespace niwa */
