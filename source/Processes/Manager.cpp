/*
 * Manager.cpp
 *
 *  Created on: 13/12/2012
 *      Author: Admin
 */

#include "Manager.h"

#include "Utilities/Logging/Logging.h"

namespace isam {
namespace processes {

Manager::Manager() {
}

Manager::~Manager() noexcept(true) {
}

/**
 * Validate any loaded processes we have.
 */
void Manager::Validate() {
  LOG_TRACE();

  if (objects_.size() == 0)
    LOG_ERROR("The configuration file requires you specify at least one type of process. E.g @recruitment, @mortality, @ageing");

  for(ProcessPtr process : objects_) {
    process->Validate();
  }

}

} /* namespace processes */
} /* namespace isam */
