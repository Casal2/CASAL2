/**
 * @file ProcessAbundance.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 17/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
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
 *
 */
ProcessAbundance::ProcessAbundance(Model* model)
   : observations::age::Abundance(model) {

  parameters_.Bind<string>(PARAM_CATCHABILITY, &catchability_label_, "The label of the catchability (q) block assumed for this observation", "");
  parameters_.Bind<string>(PARAM_OBS, &obs_, "The observed values", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years of the observed values", "");
  parameters_.Bind<Double>(PARAM_ERROR_VALUE, &error_values_, "The error values of the observed values (note the units depend on the likelihood)", "");
  parameters_.Bind<Double>(PARAM_PROCESS_ERROR, &process_error_value_, "The label of the process error", "", Double(0.0));
  parameters_.Bind<string>(PARAM_PROCESS, &process_label_, "The label of the process for the observation", "");
  parameters_.Bind<double>(PARAM_PROCESS_PROPORTION, &process_proportion_, "Proportion through the process when the observation is evaluated", "", double(0.5));

  mean_proportion_method_ = false;
}

/**
 *
 */
void ProcessAbundance::DoBuild() {
  Abundance::DoBuild();

  if (process_proportion_ < 0.0 || process_proportion_ > 1.0)
    LOG_ERROR_P(PARAM_PROCESS_PROPORTION) << ": process_proportion (" << process_proportion_ << ") must be between 0.0 and 1.0";
  proportion_of_time_ = process_proportion_;

  auto time_step = model_->managers().time_step()->GetTimeStep(time_step_label_);
  if (!time_step) {
    LOG_FATAL_P(PARAM_TIME_STEP) << time_step_label_ << " could not be found. Have you defined it?";
  } else {
    for (unsigned year : years_)
      time_step->SubscribeToProcess(this, year, process_label_);
  }
  for (auto year : years_) {
  	if((year < model_->start_year()) || (year > model_->final_year()))
  		LOG_ERROR_P(PARAM_YEARS) << "Years can't be less than start_year (" << model_->start_year() << "), or greater than final_year (" << model_->final_year() << "). Please fix this.";
  }

}

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */

