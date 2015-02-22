/**
 * @file Manager.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "Manager.h"

#include "Categories/Categories.h"

// namespaces
namespace niwa {
namespace agelengths {

/**
 *
 */
AgeLengthPtr Manager::GetAgeLength(const string& label) {
  for (AgeLengthPtr length : objects_) {
    if (length->label() == label)
      return length;
  }

  return AgeLengthPtr();
}

} /* namespace agelengths */
} /* namespace niwa */
