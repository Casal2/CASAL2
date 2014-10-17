/**
 * @file ProcessAbundance.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 17/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "ProcessAbundance.h"

#include "Processes/Manager.h"

// namespaces
namespace isam {
namespace observations {

/**
 *
 */
ProcessAbundance::ProcessAbundance() {
  parameters_.Bind<string>(PARAM_CATCHABILITY, &catchability_label_, "TBA", "");
  parameters_.Bind<string>(PARAM_OBS, &obs_, "Observation values", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years to execute in", "");
  parameters_.Bind<double>(PARAM_ERROR_VALUE, &error_values_, "The error values to use against the observation values", "");
  parameters_.Bind<double>(PARAM_DELTA, &delta_, "Delta value for error values", "", 1e-10);
  parameters_.Bind<double>(PARAM_PROCESS_ERROR, &process_error_, "Process error", "", 0.0);
  parameters_.Bind<string>(PARAM_PROCESS, &process_label_, "Process label", "");

  mean_proportion_method_ = false;
  proportion_of_time_ = 1.0;
}

/**
 *
 */
void ProcessAbundance::DoBuild() {
  Abundance::DoBuild();

  ProcessPtr process = processes::Manager::Instance().GetProcess(process_label_);
  if (!process)
    LOG_ERROR(parameters_.location(PARAM_PROCESS) << process_label_ << " could not be found. Have you defined it?");

  for (unsigned year : years_)
    process->Subscribe(year, time_step_label_, shared_ptr());
}
} /* namespace observations */
} /* namespace isam */
