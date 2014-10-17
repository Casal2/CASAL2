/**
 * @file ProcessProportionsAtAge.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 17/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "ProcessProportionsAtAge.h"

#include "Processes/Manager.h"
#include "Utilities/DoubleCompare.h"

// namespaces
namespace isam {
namespace observations {

/**
 *
 */
ProcessProportionsAtAge::ProcessProportionsAtAge() {
  parameters_.Bind<unsigned>(PARAM_MIN_AGE, &min_age_, "Minimum age", "");
  parameters_.Bind<unsigned>(PARAM_MAX_AGE, &max_age_, "Maximum age", "");
  parameters_.Bind<bool>(PARAM_AGE_PLUS, &age_plus_, "Use age plus group", "", true);
  parameters_.Bind<double>(PARAM_TOLERANCE, &tolerance_, "Tolerance", "", 0.001);
  parameters_.Bind<unsigned>(PARAM_YEAR, &year_, "Year to execute in", "");
  parameters_.Bind<string>(PARAM_OBS, &obs_, "Observation values", "");
  parameters_.Bind<double>(PARAM_ERROR_VALUE, &error_values_, "Error values", "");
  parameters_.Bind<double>(PARAM_DELTA, &delta_, "Delta", "", DELTA);
  parameters_.Bind<double>(PARAM_PROCESS_ERROR, &process_error_, "Process error", "", 0.0);
  parameters_.Bind<string>(PARAM_AGEING_ERROR, &ageing_error_label_, "Label of ageing error to use", "", "");
  parameters_.Bind<string>(PARAM_PROCESS, &process_label_, "Process label", "");

  mean_proportion_method_ = false;
  proportion_of_time_ = 1.0;
}

/**
 *
 */
void ProcessProportionsAtAge::DoBuild() {
  ProportionsAtAge::DoBuild();

  ProcessPtr process = processes::Manager::Instance().GetProcess(process_label_);
  if (!process)
    LOG_ERROR(parameters_.location(PARAM_PROCESS) << process_label_ << " could not be found. Have you defined it?");

  process->Subscribe(year_, time_step_label_, shared_ptr());
}

} /* namespace observations */
} /* namespace isam */
