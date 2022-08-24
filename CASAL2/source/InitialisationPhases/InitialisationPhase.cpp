/**
 * @file InitialisationPhase.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "InitialisationPhase.h"

#include "../TimeSteps/Manager.h"
#include "../TimeSteps/TimeStep.h"

// Namespaces
namespace niwa {

/**
 * Default constructor
 */
InitialisationPhase::InitialisationPhase(shared_ptr<Model> model) : model_(model) {
  LOG_TRACE();

  parameters_.Bind<string>(PARAM_LABEL, &label_, "The label of the initialisation phase", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "The type of initialisation", "", PARAM_ITERATIVE);
}

/**
 * Validate the initialisation phase
 *
 * 1. Check for any required parameters
 * 2. Assign local variables from parameters
 */
void InitialisationPhase::Validate() {
  parameters_.Populate(model_);
  DoValidate();
}

/**
 * Build the initialisation phase
 *
 * 1. Ensure timesteps are set up with the default processes for initialisation phases
 */
void InitialisationPhase::Build() {
  // Set the default process labels for the time step for this phase
  auto time_steps = model_->managers()->time_step()->ordered_time_steps();
  for (auto time_step : time_steps) time_step->SetInitialisationProcessLabels(label_, time_step->process_labels());

  DoBuild();
}


/**
 * Build the initialisation phase
 *
 * 1. Ensure timesteps are set up with the default processes for initialisation phases
 */
void InitialisationPhase::Execute() {
  // Set the default process labels for the time step for this phase
  DoExecute();
}

} /* namespace niwa */
