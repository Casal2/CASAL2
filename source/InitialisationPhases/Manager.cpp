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

  // Build our objects
  for(InitialisationPhasePtr phase : objects_)
    phase->Build();

  // Order our time steps based on the parameter given to the model
  vector<string> phases = Model::Instance()->initialisation_phases();
  for(string label : phases) {
    for(InitialisationPhasePtr phase : objects_) {
      if (phase->label() == label) {
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
  for(InitialisationPhasePtr phase : ordered_initialisation_phases_) {
    phase->Execute();
  }
}

} /* namespace initialisationphases */
} /* namespace isam */
