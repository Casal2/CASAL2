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

#include "DerivedQuantities/Manager.h"
#include "TimeSteps/Factory.h"
#include "Processes/Manager.h"

// Namespaces
namespace isam {

/**
 * Default constructor
 */
InitialisationPhase::InitialisationPhase() {
  LOG_TRACE();

  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "");
  parameters_.Bind<string>(PARAM_PROCESSES, &process_labels_, "A list of processes to execute during this phase", "", true);
  parameters_.Bind<string>(PARAM_TIME_STEPS, &time_step_labels_, "A list of time steps to execute during this phase", "", true);
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The number of iterations to execute this phase for", "");
}

/**
 * Validate our initialisation phase.
 *
 * 1. Check for any required parameters
 * 2. Assign local variables from parameters
 */
void InitialisationPhase::Validate() {
  parameters_.Populate();

  if (time_step_labels_.size() == 0 && process_labels_.size() == 0) {
    LOG_ERROR(parameters_.location(PARAM_LABEL) << " must define either " << PARAM_PROCESSES << " or " << PARAM_TIME_STEPS);
  } else if (time_step_labels_.size() > 0 && process_labels_.size() > 0) {
    LOG_ERROR(parameters_.location(PARAM_PROCESSES) << " cannot be defined for an initialisation phase if you have also defined " << PARAM_TIME_STEPS);
  }

  // Create a new time step if we need too.
  if (process_labels_.size() != 0) {
    LOG_INFO("Initialisation phase needs to create a new time step: " << string(PARAM_INITIALISATION_PHASE) + string(".") + label_);
    TimeStepPtr time_step = timesteps::Factory::Create();
    time_step->parameters().Add(PARAM_LABEL, string(PARAM_INITIALISATION) + string(".") + label_, __FILE__, __LINE__);
    time_step->parameters().Add(PARAM_PROCESSES, process_labels_, __FILE__, __LINE__);

    time_steps_.push_back(time_step);
  }
}

/**
 * Build our runtime pointers
 */
void InitialisationPhase::Build() {
  LOG_TRACE();

  /**
   * only get time steps if we didn't specify processes manually
   */
  if (process_labels_.size() == 0) {
    timesteps::Manager& time_step_manager = timesteps::Manager::Instance();
    TimeStepPtr time_step;
    for (string label : time_step_labels_) {

      time_step = time_step_manager.GetTimeStep(label);
      if (!time_step)
        LOG_ERROR(parameters_.location(PARAM_TIME_STEPS) << " (" << label << ") has not been defined as a time step. Please ensure you have defined it");

      time_steps_.push_back(time_step);
    }
  }
}

/**
 * Execute the timesteps we have.
 */
void InitialisationPhase::Execute() {
  LOG_INFO("Executing " << years_ << " years with " << process_labels_.size() << " processes");
  for (unsigned year = 0; year < years_; ++year) {
    for (TimeStepPtr time_step : time_steps_) {
      time_step->Execute();
      time_step->ExecuteInitialisationDerivedQuantities(index_);
    }
  }
}


} /* namespace isam */
