/**
 * @file ProcessBiomass.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 17/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 */

// headers
#include "ProcessBiomass.h"

#include "Processes/Manager.h"

// namespaces
namespace isam {
namespace observations {

/**
 *
 */
ProcessBiomass::ProcessBiomass() {
  parameters_.Bind<string>(PARAM_CATCHABILITY, &catchability_label_, "TBA", "");
  parameters_.Bind<string>(PARAM_OBS, &obs_, "Observation values", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years to execute in", "");
  parameters_.Bind<Double>(PARAM_ERROR_VALUE, &error_values_, "The error values to use against the observation values", "");
  parameters_.Bind<Double>(PARAM_DELTA, &delta_, "Delta value for error values", "", 1e-10);
  parameters_.Bind<Double>(PARAM_PROCESS_ERROR, &process_error_, "Process error", "", 0.0);
  parameters_.Bind<string>(PARAM_PROCESS, &process_label_, "Process label", "");
  parameters_.Bind<Double>(PARAM_PROCESS_PROPORTION, &process_proportion_, "Process proportion", "", 0.5);

  mean_proportion_method_ = false;
}

/**
 *
 */
void ProcessBiomass::DoBuild() {
  Biomass::DoBuild();

  if (process_proportion_ < 0.0 || process_proportion_ > 1.0)
    LOG_ERROR(parameters_.location(PARAM_PROCESS_PROPORTION) << ": process_proportion (" << AS_DOUBLE(process_proportion_) << ") must be between 0.0 and 1.0");
  proportion_of_time_ = process_proportion_;

  ProcessPtr process = processes::Manager::Instance().GetProcess(process_label_);
  if (!process)
    LOG_ERROR(parameters_.location(PARAM_PROCESS) << " " << process_label_ << " could not be found. Have you defined it?");

  for (unsigned year : years_)
    process->Subscribe(year, time_step_label_, shared_ptr());
}
} /* namespace observations */
} /* namespace isam */
