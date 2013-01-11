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
 *
 */
void Manager::Validate() {
  LOG_TRACE();

  for(InitialisationPhasePtr phase : objects_) {
    phase->Validate();
  }

}

} /* namespace initialisationphases */
} /* namespace isam */
