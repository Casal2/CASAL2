/**
 * @file InitialisationPhase.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "InitialisationPhase.h"

#include "TimeSteps/Factory.h"
#include "Processes/Manager.h"

// Namespaces
namespace isam {

/**
 * Default constructor
 */
InitialisationPhase::InitialisationPhase() {
  LOG_TRACE();

  parameters_.RegisterAllowed(PARAM_LABEL);
  parameters_.RegisterAllowed(PARAM_PROCESSES);
  parameters_.RegisterAllowed(PARAM_YEARS);
}

/**
 * Validate our initialisation phase.
 *
 * 1. Check for any required parameters
 * 2. Assign local variables from parameters
 */
void InitialisationPhase::Validate() {
  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_PROCESSES);
  CheckForRequiredParameter(PARAM_YEARS);

  label_            = parameters_.Get(PARAM_LABEL).GetValue<string>();
  process_labels_   = parameters_.Get(PARAM_PROCESSES).GetValues<string>();
  years_            = parameters_.Get(PARAM_YEARS).GetValue<unsigned>();
}

/**
 * Build our runtime pointers
 */
void InitialisationPhase::Build() {
  time_step_ = timesteps::Factory::Create();
  time_step_->parameters().Add(PARAM_LABEL, "Auto-generated", __FILE__, __LINE__);
  time_step_->parameters().Add(PARAM_PROCESSES, process_labels_, __FILE__, __LINE__);
  time_step_->Validate();
  time_step_->Build();

  processes::Manager& process_manager = processes::Manager::Instance();
  for(string label : process_labels_) {
    if (!process_manager.GetProcess(label))
      LOG_ERROR(parameters_.location(PARAM_PROCESSES) << "(" << label << ") has not been defined as a process. Please ensure you have defined it");
  }
}

/**
 * Execute the timesteps we have.
 */
void InitialisationPhase::Execute() {
  LOG_INFO("Executing " << years_ << " years with " << process_labels_.size() << " processes");
  for (unsigned year = 0; year < years_; ++year)
    time_step_->Execute();
}


} /* namespace isam */
