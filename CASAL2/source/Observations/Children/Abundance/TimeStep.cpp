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
#include "Utilities/DoubleCompare.h"

// namespaces
namespace niwa {
namespace observations {

/**
 *
 */
TimeStepAbundance::TimeStepAbundance(Model* model)
  : observations::Abundance(model) {
  parameters_.Bind<string>(PARAM_CATCHABILITY, &catchability_label_, "The label of the catchability coefficient (q) assumed", "");
  parameters_.Bind<string>(PARAM_OBS, &obs_, "The observed values", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years for which there are observations", "");
  parameters_.Bind<Double>(PARAM_ERROR_VALUE, &error_values_, "The error values of the observed values (note the units depend on the likelihood)", "");
  parameters_.Bind<Double>(PARAM_PROCESS_ERROR, &process_error_value_, "The label of the process error block", "", Double(0.0));
  parameters_.Bind<Double>(PARAM_TIME_STEP_PROPORTION, &time_step_proportion_, "Proportion through the mortality block of the time step when the observation is evaluated", "", Double(0.5));

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
