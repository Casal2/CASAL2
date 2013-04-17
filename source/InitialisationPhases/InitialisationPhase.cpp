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

#include "TimeSteps/Manager.h"

// Namespaces
namespace isam {

/**
 * Default constructor
 */
InitialisationPhase::InitialisationPhase() {
  LOG_TRACE();

  parameters_.RegisterAllowed(PARAM_LABEL);
  parameters_.RegisterAllowed(PARAM_TIME_STEPS);
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
  CheckForRequiredParameter(PARAM_TIME_STEPS);
  CheckForRequiredParameter(PARAM_YEARS);

  label_            = parameters_.Get(PARAM_LABEL).GetValue<string>();
  time_step_names_  = parameters_.Get(PARAM_TIME_STEPS).GetValues<string>();
  years_            = parameters_.Get(PARAM_YEARS).GetValue<unsigned>();
}

/**
 * Build our runtime pointers
 */
void InitialisationPhase::Build() {
  timesteps::Manager& time_steps_manager = timesteps::Manager::Instance();
  for (const string& label : time_step_names_) {
    TimeStepPtr time_step = time_steps_manager.GetTimeStep(label);
    if (!time_step)
      LOG_ERROR(parameters_.location(PARAM_TIME_STEPS) << ": time step " << label << " does not exist. Have you defined it?");

    time_steps_.push_back(time_step);
  }
}

/**
 * Execute the timesteps we have.
 */
void InitialisationPhase::Execute() {
  LOG_TRACE();
  for (unsigned year = 0; year < years_; ++year) {
    for (TimeStepPtr time_step : time_steps_) {
      time_step->Execute();
    }
  }
}


} /* namespace isam */
