/**
 * @file Manager.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/02/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Manager.h"

// Namespaces
namespace niwa {
namespace penalties {

/**
 * Default Constructor
 */
Manager::Manager() {}

/**
 * Return the penalty with a specified label
 *
 * @param label The label of the penalty
 * @return a pointer to the Penalty
 */
Penalty* Manager::GetPenalty(const string& label) const {
  for (auto penalty : objects_) {
    if (penalty->label() == label) {
      return penalty;
    }
  }

  return nullptr;
}

/**
 * Get a pointer to a "process" type penalty.
 *
 * @param label The label of the penalty
 * @return shared_ptr to penalty or empty shared_ptr if not found
 * 
 */
penalties::Process* Manager::GetProcessPenalty(const string& label) {
  for (Penalty* penalty : objects_) {
    if (penalty->type() == PARAM_PROCESS && penalty->label() == label) {
      return dynamic_cast<penalties::Process*>(penalty);
    }
  }

  return nullptr;
}

} /* namespace penalties */
} /* namespace niwa */
