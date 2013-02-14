/**
 * @file TimeStep.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 20/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

#include "TimeStep.h"

#include "Processes/Manager.h"

namespace isam {

/**
 * Default Constructor
 */
TimeStep::TimeStep() {
  LOG_TRACE();

  parameters_.RegisterAllowed(PARAM_LABEL);
  parameters_.RegisterAllowed(PARAM_PROCESSES);
}

/**
 * Validate our time step
 */
void TimeStep::Validate() {
  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_PROCESSES);

  label_          = parameters_.Get(PARAM_LABEL).GetValue<string>();
  process_names_  = parameters_.Get(PARAM_PROCESSES).GetValues<string>();
}

/**
 * Build our time step
 */
void TimeStep::Build() {

  // Get the pointers to our processes
  processes::Manager& process_manager = processes::Manager::Instance();
  for (string process_name : process_names_) {
    ProcessPtr process = process_manager.GetProcess(process_name);
    if (!process)
      LOG_ERROR(parameters_.location(PARAM_PROCESSES) << ": process " << process_name << " does not exist. Have you defined it?");

    processes_.push_back(process);
  }
}

/**
 * Execute the time step
 */
void TimeStep::Execute() {
  for (ProcessPtr process : processes_)
    process->Execute();
}


} /* namespace isam */
