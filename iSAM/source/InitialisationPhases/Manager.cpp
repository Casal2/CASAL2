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

// Namespaces
namespace isam {
namespace initialisationphases {

Manager::Manager() {
}

Manager::~Manager() noexcept(true) {
}

/**
 *
 */
void Manager::Build() {
  LOG_TRACE();

  // Build our objects
  for(InitialisationPhasePtr phase : objects_)
    phase->Build();

  unsigned index = 0;

  // Order our time steps based on the parameter given to the model
  ordered_initialisation_phases_.clear();

  LOG_INFO("Ordering initialisation phases");
  vector<string> phases = Model::Instance()->initialisation_phases();
  for(string label : phases) {
    for(InitialisationPhasePtr phase : objects_) {
      if (phase->label() == label) {
        phase->set_index(index++);
        ordered_initialisation_phases_.push_back(phase);
        break;
      }
    }
  }
}

/**
 * Execute all of the time steps in the current phase
 */
void Manager::Execute() {
  LOG_TRACE();

  last_executed_phase_ = 0;
  for (unsigned i = 0; i < ordered_initialisation_phases_.size(); ++i) {
    ordered_initialisation_phases_[i]->Execute();
    last_executed_phase_ = i;
  }
}

/**
 *
 */
bool Manager::IsPhaseDefined(const string& label) const {
  for(InitialisationPhasePtr phase : objects_)
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

  LOG_ERROR("The initialisation phase " << label << " has not been defined in the active list of initialisation phases");
  return 0;
}

} /* namespace initialisationphases */
} /* namespace isam */
