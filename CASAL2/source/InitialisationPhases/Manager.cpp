/**
 * @file Manager.cpp
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
#include "Manager.h"

#include "Model/Model.h"
#include "TimeSteps/Manager.h"

// Namespaces
namespace niwa {
namespace initialisationphases {

Manager::Manager() {
}

Manager::~Manager() noexcept(true) {
}

/**
 *
 */
void Manager::Build() {
  LOG_CODE_ERROR() << "This method is not supported";
}

/**
 *
 */
void Manager::Validate() {
  bool has_derived_or_iterative = false;
  for (auto phase : objects_) {
    phase->Validate();
    has_derived_or_iterative = phase->type() == PARAM_ITERATIVE || phase->type() == PARAM_DERIVED || has_derived_or_iterative;
    if (phase->type() == PARAM_CINITIAL && !has_derived_or_iterative) {
      LOG_ERROR() << phase->location() << " cannot be defined unless you have defined an iterative or derived initialisation phase prior in the annual cycle";
    }
  }
}

/**
 *
 */
void Manager::Build(Model* model) {
  LOG_TRACE();

  // Build our objects
  for(auto phase : objects_)
    phase->Build();

  // Order our time steps based on the parameter given to the model
  ordered_initialisation_phases_.clear();

  LOG_FINE() << "Ordering initialisation phases";
  vector<string> phases = model->initialisation_phases();
  for(string label : phases) {
    for(auto phase : objects_) {
      if (phase->label() == label) {
        ordered_initialisation_phases_.push_back(phase);
        break;
      }
    }
  }

  for (auto time_step : model->managers().time_step()->ordered_time_steps())
    time_step->BuildInitialisationProcesses();
}

/**
 * Execute all of the time steps in the current phase
 */
void Manager::Execute() {
  LOG_TRACE();

  last_executed_phase_ = 0;
  for (current_initialisation_phase_ = 0; current_initialisation_phase_ < ordered_initialisation_phases_.size(); ++current_initialisation_phase_) {
    ordered_initialisation_phases_[current_initialisation_phase_]->Execute();
    last_executed_phase_ = current_initialisation_phase_;
  }
}

/**
 *
 */
bool Manager::IsPhaseDefined(const string& label) const {
  for(auto phase : objects_)
    if (phase->label() == label)
      return true;

  return false;
}

/**
 *
 */
unsigned Manager::GetPhaseIndex(const string& label) const {
  for (unsigned i = 0; i < ordered_initialisation_phases_.size(); ++i) {
    if (ordered_initialisation_phases_[i]->label() == label)
      return i;
  }

  LOG_ERROR() << "The initialisation phase " << label << " has not been defined in the active list of initialisation phases";
  return 0;
}


/**
 * Return the InitialisationPhase with the name passed in as a parameter.
 * If no InitialisationPhase is found then an empty pointer will
 * be returned.
 *
 * @param label The name of the process to find
 * @return A pointer to the InitialisationPhase or empty pointer
 */
InitialisationPhase* Manager::GetInitPhase(const string& label) {
  for (auto GetInitPhase : objects_) {
    if (GetInitPhase->label() == label)
      return GetInitPhase;
  }

  return nullptr;
}

} /* namespace initialisationphases */
} /* namespace niwa */
