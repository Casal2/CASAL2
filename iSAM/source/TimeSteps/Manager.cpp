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
#include "Reports/Manager.h"

// Namespaces
namespace niwa {
namespace timesteps {

/**
 * Default Constructor
 */
Manager::Manager() {
  LOG_INFO("TimeStep Manager Constructor");
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
 *
 */
unsigned Manager::GetTimeStepIndex(const string& time_step_label) const {
  unsigned index = 9999;

  for (index = 0; index < ordered_time_steps_.size(); ++index) {
    if (ordered_time_steps_[index]->label() == time_step_label)
      return index;
  }

  LOG_ERROR("The time step " << time_step_label << " could not be found");
}

/**
 *
 */
unsigned Manager::GetTimeStepIndexForProcess(const string& process_label) const {
  unsigned index = 9999;

  for (index = 0; index < ordered_time_steps_.size(); ++index) {
    const vector<string>& process_labels = ordered_time_steps_[index]->process_labels();
    if (std::find(process_labels.begin(), process_labels.end(), process_label) != process_labels.end())
      return index;
  }

  LOG_ERROR("The process " << process_label << " could not be found in any of the time steps");
}

/**
 *
 */
vector<unsigned> Manager::GetTimeStepIndexesForProcess(const string& process_label) const {
  vector<unsigned> result;

  for (unsigned index = 0; index < ordered_time_steps_.size(); ++index) {
    if (ordered_time_steps_[index]->HasProcess(process_label))
      result.push_back(index);
  }

  return result;
}

/**
 * Return a vector that contains the different types or processes and wthe order
 * they are executed in. This is primarily used by the BH Recruitment to determine the SSB Offset
 *
 * @return Vector of ProcessType::Types for all processes executed.
 */
vector<ProcessType> Manager::GetOrderedProcessTypes() {
  vector<ProcessType> types;
  for (TimeStepPtr time_step : ordered_time_steps_) {
    for (ProcessPtr process : time_step->processes()) {
      types.push_back(process->process_type());
    }
  }

  return types;
}

/**
 * Validate our Time Steps
 */
void Manager::Validate() {
  LOG_TRACE();

  for (TimeStepPtr time_step : objects_) {
    LOG_INFO("Validating time step: " << time_step->label());
    time_step->Validate();
  }

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

  LOG_INFO("ordered_time_steps_.size(): " << ordered_time_steps_.size());
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
}

/**
 * Execute all of the timesteps
 * for the current year.
 */
void Manager::Execute(unsigned year) {
  LOG_TRACE();

  reports::Manager& report_manager = reports::Manager::Instance();
  for (current_time_step_ = 0; current_time_step_ < ordered_time_steps_.size(); ++current_time_step_) {
    LOG_INFO("Current Time Step: " <<  current_time_step_);
    ordered_time_steps_[current_time_step_]->Execute(year);
    report_manager.Execute(year, ordered_time_steps_[current_time_step_]->label());
  }

  // reset this for age sizes
  current_time_step_ = 0;
}

/**
 * Execute all of the timesteps
 * for the current year.
 */
void Manager::ExecuteInitialisation(const string& phase_label, unsigned years) {
  LOG_TRACE();

  for (unsigned i = 0; i < years; ++i) {
    for (current_time_step_ = 0; current_time_step_ < ordered_time_steps_.size(); ++current_time_step_) {
      ordered_time_steps_[current_time_step_]->ExecuteForInitialisation(phase_label);
    }
  }

  // reset this for age sizes
  current_time_step_ = 0;
}

} /* namespace timesteps */
} /* namespace niwa */
