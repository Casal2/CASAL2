/**
 * @file Manager.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 13/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Manager.h"

#include "DerivedQuantities/Manager.h"
#include "Model/Model.h"
#include "Observations/Manager.h"
#include "Reports/Manager.h"

// Namespaces
namespace isam {
namespace timesteps {

/**
 * Default Constructor
 */
Manager::Manager() {
}

/**
 * Destructor
 */
Manager::~Manager() noexcept(true) {
}

/**
 *
 */
TimeStepPtr Manager::GetTimeStep(const string& label) const {
  LOG_INFO("label: " << label);
  TimeStepPtr result;
  for (TimeStepPtr time_step : objects_) {
    if (time_step->label() == label) {
      result = time_step;
      break;
    }
  }

  return result;
}

/**
 * Validate our Time Steps
 */
void Manager::Validate() {
  LOG_TRACE();

  for (TimeStepPtr time_step : objects_) {
    time_step->Validate();
  }
}

/**
 * Build our time step manager.
 *
 * Get the time steps and order them.
 */
void Manager::Build() {

  // Build our objects
  for(TimeStepPtr time_step : objects_)
    time_step->Build();

  // Order our time steps based on the parameter given to the model
  vector<string> time_steps = Model::Instance()->time_steps();
  for(string time_step_label : time_steps) {
    for(TimeStepPtr time_step : objects_) {
      if (time_step->label() == time_step_label) {
        ordered_time_steps_.push_back(time_step);
        break;
      }
    }
  }
}

/**
 * Execute all of the timesteps
 * for the current year.
 */
void Manager::Execute(unsigned year) {
  LOG_TRACE();

  reports::Manager& report_manager = reports::Manager::Instance();
  observations::Manager& observations_manager = observations::Manager::Instance();

  for (TimeStepPtr time_step : ordered_time_steps_) {
    observations_manager.PreExecute(year, time_step->label());

    time_step->Execute();

    observations_manager.Execute(year, time_step->label());
    report_manager.Execute(year, time_step->label());
  }
}

} /* namespace timesteps */
} /* namespace isam */
