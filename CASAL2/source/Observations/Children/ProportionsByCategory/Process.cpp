/**
 * @file ProcessProportionsByCategory.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 17/02/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */

// headers
#include "Process.h"

#include "TimeSteps/Manager.h"
#include "Utilities/DoubleCompare.h"

// namespaces
namespace niwa {
namespace observations {

/**
 * Default constructor
 */
ProcessProportionsByCategory::ProcessProportionsByCategory(Model* model)
  : observations::ProportionsByCategory(model) {
  parameters_.Bind<string>(PARAM_PROCESS, &process_label_, "The label of the process for the observation", "");
  parameters_.Bind<Double>(PARAM_PROCESS_PROPORTION, &process_proportion_, "Proportion through the process when the observation is evaluated", "", Double(0.5));
  mean_proportion_method_ = false;
}

/**
 * Build our process proportions by category
 */
void ProcessProportionsByCategory::DoBuild() {
  ProportionsByCategory::DoBuild();

  if (process_proportion_ < 0.0 || process_proportion_ > 1.0)
    LOG_ERROR_P(PARAM_PROCESS_PROPORTION) << ": process_proportion (" << AS_DOUBLE(process_proportion_) << ") must be between 0.0 and 1.0";
  proportion_of_time_ = process_proportion_;

  auto time_step = model_->managers().time_step()->GetTimeStep(time_step_label_);
  if (!time_step) {
    LOG_FATAL_P(PARAM_TIME_STEP) << time_step_label_ << " could not be found. Have you defined it?";
  } else {
    for (unsigned year : years_)
      time_step->SubscribeToProcess(this, year, process_label_);
  }
}

} /* namespace observations */
} /* namespace niwa */
