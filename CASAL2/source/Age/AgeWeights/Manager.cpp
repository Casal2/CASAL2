/**
 * @file Manager.cpp
 * @author  C.Marsh
 * @date 16/11/2017
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 *
 */

// headers
#include "../AgeWeights/Manager.h"

#include "Common/Categories/Categories.h"

// namespaces
namespace niwa {
namespace ageweights {

/**
 * Return an age weight smart_ptr based on label
 *
 * @param label The label of the age weight object
 * @return age weight smart_ptr
 */
AgeWeight* Manager::FindAgeWeight(const string& label) {
  for (auto length : objects_) {
    if (length->label() == label)
      return length;
  }

  return nullptr;
}

} /* namespace ageweights */
} /* namespace niwa */
