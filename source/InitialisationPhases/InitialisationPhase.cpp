/*
 * InitialisationPhase.cpp
 *
 *  Created on: 13/12/2012
 *      Author: Admin
 */

#include "InitialisationPhase.h"

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
 * Execute the timesteps we have.
 */
void InitialisationPhase::Execute() {
  for (unsigned year = 0; year < years_; ++year) {
    for (TimeStepPtr time_step : time_steps_) {
      time_step->Execute();
    }
  }
}


} /* namespace isam */
