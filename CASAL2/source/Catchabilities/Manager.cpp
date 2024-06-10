/**
 * @file Manager.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 12/03/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "Manager.h"

// namespaces
namespace niwa {
namespace catchabilities {

/**
 * Find and return pointer to catchability
 *
 * @param label The label of the catchability
 * @return Catchability pointer
 */
Catchability* Manager::GetCatchability(const string& label) {
  for (auto catchability : objects_) {
    if (catchability->label() == label)
      return catchability;
  }

  return nullptr;
}

} /* namespace catchabilities */
} /* namespace niwa */
