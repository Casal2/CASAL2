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
namespace age {

/**
 * Default constructor
 */
ProcessProportionsByCategory::ProcessProportionsByCategory(Model* model)
   : observations::age::ProportionsByCategory(model) {
  parameters_.Bind<string>(PARAM_PROCESS, &process_label_, "The label of the process for the observation", "");
  parameters_.Bind<double>(PARAM_PROCESS_PROPORTION, &process_proportion_, "Proportion through the mortality block of the time step when the observation is evaluated", "", double(0.5))->set_range(0.0, 1.0);

  mean_proportion_method_ = false;
}

/**
 * Build our process proportions by category
 */
void ProcessProportionsByCategory::DoBuild() {
  ProportionsByCategory::DoBuild();

  if (process_proportion_ < 0.0 || process_proportion_ > 1.0)
    LOG_ERROR_P(PARAM_PROCESS_PROPORTION) << ": process_proportion (" << process_proportion_ << ") must be between 0.0 and 1.0 inclusive";
  proportion_of_time_ = process_proportion_;

  auto time_step = model_->managers().time_step()->GetTimeStep(time_step_label_);
  if (!time_step) {
    LOG_FATAL_P(PARAM_TIME_STEP) << "Time step label " << time_step_label_ << " was not found.";
  } else {
    for (unsigned year : years_)
      time_step->SubscribeToProcess(this, year, process_label_);
  }
}

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */
