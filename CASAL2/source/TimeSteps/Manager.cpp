/**
 * @file Manager.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 13/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Manager.h"

#include "../DerivedQuantities/Manager.h"
#include "../Model/Model.h"
#include "../Reports/Manager.h"

// Namespaces
namespace niwa {
namespace timesteps {

/**
 * Default Constructor
 */
Manager::Manager() {
  LOG_FINE() << "TimeStep Manager Constructor";
}

/**
 * Destructor
 */
Manager::~Manager() noexcept(true) {}

/**
 * Get the time step
 *
 * @param label The label of the time step
 * @return a pointer to the TimeStep
 */
TimeStep* Manager::GetTimeStep(const string& label) const {
  LOG_FINE() << "label: " << label;
  TimeStep* result = nullptr;
  for (auto time_step : objects_) {
    if (time_step->label() == label) {
      result = time_step;
      break;
    }
  }

  LOG_FINE() << "returning: " << result;
  return result;
}

/**
 * Get the index of the time step
 *
 * @param time_step_label The label of the time step
 * @return the index of the time step, or 0 if not found
 */
unsigned Manager::GetTimeStepIndex(const string& time_step_label) const {
  unsigned index = 9999;

  for (index = 0; index < ordered_time_steps_.size(); ++index) {
    if (ordered_time_steps_[index]->label() == time_step_label)
      return index;
  }

  LOG_ERROR() << "The time step " << time_step_label << " was not found";
  return 0;
}

/**
 * Check that the time step exists
 *
 * @param time_step_label The label of the time step
 * @return true if it exists, false if not
 */
bool Manager::CheckTimeStep(const string& time_step_label) const {
  bool time_step_exists = false;

  for (unsigned index = 0; index < ordered_time_steps_.size(); ++index) {
    if (ordered_time_steps_[index]->label() == time_step_label)
      time_step_exists = true;
  }
  return time_step_exists;
}

/**
 * Get the time step index for a process
 *
 * @param process_label The label of the process
 * @return the index of the time step, or 0 if not found
 */
unsigned Manager::GetTimeStepIndexForProcess(const string& process_label) const {
  unsigned index = 9999;

  for (index = 0; index < ordered_time_steps_.size(); ++index) {
    const vector<string>& process_labels = ordered_time_steps_[index]->process_labels();
    if (std::find(process_labels.begin(), process_labels.end(), process_label) != process_labels.end())
      return index;
  }

  LOG_ERROR() << "The process " << process_label << " was not found in any of the time steps";
  return 0;
}

/**
 * Return a vector of time step index values for a process
 *
 * @param process_label The label of the process
 * @return the vector of index values of the time steps, or an empty vector if not found
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
 * Return a vector that contains the different types or processes and the order
 * they are executed in. This is primarily used by the BH Recruitment to determine the SSB Offset
 *
 * @return Vector of ProcessType::Types for all processes executed.
 */
vector<ProcessType> Manager::GetOrderedProcessTypes() {
  vector<ProcessType> types;
  for (auto time_step : ordered_time_steps_) {
    for (auto process : time_step->processes()) {
      types.push_back(process->process_type());
    }
  }

  return types;
}

/**
 *  Return the sequence in which the process was executed in the entire annual cycle
 *  This treats the annual cycle as continuous.
 *
 * @param process_label The label of the process
 * @return the index of the time step, or 0 if not found
 */
unsigned Manager::GetProcessIndex(const string& process_label) const {
  unsigned index = 0;
  for (auto time_step : ordered_time_steps_) {
    for (auto process : time_step->processes()) {
      index++;
      if (process->label() == process_label)
        return index;
    }
  }

  LOG_ERROR() << "The process with the label " << process_label << " was not found in the annual cycle";
  return 0;
}

/**
 * Validate the time step objects - no model
 */
void Manager::Validate() {
  LOG_CODE_ERROR() << "This method is not supported";
}

void Manager::Validate(shared_ptr<Model> model) {
  LOG_TRACE();
  base::Manager<niwa::timesteps::Manager, niwa::TimeStep>::Validate();
  model_ = model;

  // Order our time steps based on the parameter given to the model
  vector<string> time_steps = model->time_steps();
  for (string time_step_label : time_steps) {
    for (auto time_step : objects_) {
      if (time_step->label() == time_step_label) {
        ordered_time_steps_.push_back(time_step);
        break;
      }
    }
  }

  LOG_FINEST() << "ordered_time_steps_.size(): " << ordered_time_steps_.size();
}

/**
 * Build the time step manager
 *
 * Get the time steps and order them
 */
void Manager::Build() {
  // Build our objects
  for (auto time_step : objects_) time_step->Build();
}

/**
 * Execute all of the time steps for a specific year
 *
 * @param year The year
 */
void Manager::Execute(unsigned year) {
  LOG_TRACE();

  //  auto report_manager = model_->managers()->report();
  for (current_time_step_ = 0; current_time_step_ < ordered_time_steps_.size(); ++current_time_step_) {
    LOG_FINE() << "Current Time Step: " << current_time_step_;
    ordered_time_steps_[current_time_step_]->Execute(year);

    model_->managers()->report()->Execute(model_, year, ordered_time_steps_[current_time_step_]->label());
  }

  // reset this for age sizes
  current_time_step_ = 0;
}

/**
 * Execute all of the time steps for a number of years
 *
 * @param phase_label The label of the phase
 * @param year The number of years
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

/**
 * Return the current time step index
 *
 * @return the current time step
 */
unsigned Manager::current_time_step() const {
  if (model_->state() != State::kInitialise && model_->state() != State::kExecute)
    LOG_CODE_ERROR() << "Model State is not Init or Execute. It is: " << (unsigned)model_->state();

  return current_time_step_;
}

} /* namespace timesteps */
} /* namespace niwa */
