/*
 * Manager.cpp
 *
 *  Created on: 13/12/2012
 *      Author: Admin
 */

#include "Manager.h"

namespace isam {
namespace timesteps {

Manager::Manager() {
  // TODO Auto-generated constructor stub

}

Manager::~Manager() noexcept(true) {
  // TODO Auto-generated destructor stub
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

} /* namespace timesteps */
} /* namespace isam */
