/**
 * @file Manager.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 12/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "Manager.h"

// namespaces
namespace niwa {
namespace agelengthkeys {

/**
 * Get an age length key pointer from the manager
 *
 * @param label The label of the age length key
 * @return age length key pointer, or empty pointer
 */
AgeLengthKeyPtr Manager::GetAgeLengthKey(const string& label) {
  for (AgeLengthKeyPtr length : objects_) {
    if (length->label() == label)
      return length;
  }

  return AgeLengthKeyPtr();
}

} /* namespace agelengthkeys */
} /* namespace niwa */
