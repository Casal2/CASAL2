/*
 * Manager.cpp
 *
 *  Created on: 13/12/2012
 *      Author: Admin
 */

#include "Manager.h"

namespace isam {
namespace initialisationphases {

Manager::Manager() {
}

Manager::~Manager() noexcept(true) {
}

/**
 * Execute all of the time steps in the current phase
 */
void Manager::Execute() {
  for(InitialisationPhasePtr phase : objects_) {
    phase->Execute();
  }
}

} /* namespace initialisationphases */
} /* namespace isam */
