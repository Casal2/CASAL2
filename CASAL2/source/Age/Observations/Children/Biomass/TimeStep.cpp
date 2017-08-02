/**
 * @file TimeStepBiomass.cpp
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

#include "Common/TimeSteps/Manager.h"

// namespaces
namespace niwa {
namespace age {
namespace observations {

/**
 *
 */
TimeStepBiomass::TimeStepBiomass(Model* model)
  : observations::Biomass(model) {
  parameters_.Bind<Double>(PARAM_TIME_STEP_PROPORTION, &time_step_proportion_, "Proportion through the mortality block of the time step when the observation is evaluated", "", Double(0.5));

  mean_proportion_method_ = true;
}

/**
 *
 */
void TimeStepBiomass::DoBuild() {
  Biomass::DoBuild();

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
  for (auto year : years_) {
  	if((year < model_->start_year()) || (year > model_->final_year()))
  		LOG_ERROR_P(PARAM_YEARS) << "Years can't be less than start_year (" << model_->start_year() << "), or greater than final_year (" << model_->final_year() << "). Please fix this.";
  }

}

} /* namespace observations */
} /* namespace age */
} /* namespace niwa */
