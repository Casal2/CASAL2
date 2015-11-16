/**
 * @file ProcessProportionsAtLength.cpp
 * @author C Marsh
 * @github https://github.com/Zaita
 * @date 14/08/15
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */

// headers
#include "ProcessProportionsAtLength.h"

#include "TimeSteps/Manager.h"
#include "Utilities/DoubleCompare.h"

// namespaces
namespace niwa {
namespace observations {

/**
 *
 */
ProcessProportionsAtLength::ProcessProportionsAtLength(Model* model)
  : observations::ProportionsAtLength(model) {
  parameters_.Bind<string>(PARAM_PROCESS, &process_label_, "Process label", "");
  parameters_.Bind<Double>(PARAM_PROCESS_PROPORTION, &process_proportion_, "Process proportion", "", Double(0.5));

  mean_proportion_method_ = false;
}

/**
 *
 */
void ProcessProportionsAtLength::DoBuild() {
  ProportionsAtLength::DoBuild();

  if (process_proportion_ < 0.0 || process_proportion_ > 1.0)
    LOG_ERROR_P(PARAM_PROCESS_PROPORTION) << ": process_proportion (" << AS_DOUBLE(process_proportion_) << ") must be between 0.0 and 1.0";
  proportion_of_time_ = process_proportion_;

  TimeStep* time_step = model_->managers().time_step()->GetTimeStep(time_step_label_);
  if (!time_step) {
    LOG_FATAL_P(PARAM_TIME_STEP) << time_step_label_ << " could not be found. Have you defined it?";
  } else {
    for (unsigned year : years_)
      time_step->SubscribeToProcess(this, year, process_label_);
  }
}

} /* namespace observations */
} /* namespace niwa */
