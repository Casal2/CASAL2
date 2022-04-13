/**
 * @file Manager.cpp
 * @author  C.Marsh
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 */

// headers
#include "Manager.h"

// namespaces
namespace niwa {
namespace growthincrements {

/**
 * Default constructor
 */
Manager::Manager() {}

/**
 * Return the LengthWeight pointer
 *
 * @param label The label of the object
 * @return pointer to LengthWeight
 */
GrowthIncrement* Manager::GetGrowthIncrement(const string& label) const {
  for (auto growth_increment : objects_) {
    if (growth_increment->label() == label)
      return growth_increment;
  }

  return nullptr;
}

} /* namespace lengthweights */
} /* namespace niwa */
