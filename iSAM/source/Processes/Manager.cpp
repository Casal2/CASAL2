/*
 * Manager.cpp
 *
 *  Created on: 13/12/2012
 *      Author: Admin
 */

#include "Manager.h"

#include "Utilities/Logging/Logging.h"

namespace niwa {
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

  for (ProcessPtr process : objects_) {
    process->Validate();
  }
}

/**
 * Return the process with the name passed in as a parameter.
 * If no process is found then an empty pointer will
 * be returned.
 *
 * @param label The name of the process to find
 * @return A pointer to the process or empty pointer
 */
ProcessPtr Manager::GetProcess(const string& label) {

  for (ProcessPtr process : objects_) {
    if (process->label() == label)
      return process;
  }

  return ProcessPtr();
}

} /* namespace processes */
} /* namespace niwa */
