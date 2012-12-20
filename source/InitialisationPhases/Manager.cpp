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
  // TODO Auto-generated constructor stub

}

Manager::~Manager() noexcept(true) {
  // TODO Auto-generated destructor stub
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
