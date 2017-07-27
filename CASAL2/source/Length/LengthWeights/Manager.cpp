/**
 * @file Manager.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 */

// headers
#include "Manager.h"

// namespaces
namespace niwa {
namespace lengthweights {

/**
 * class definition
 */
Manager::Manager() {
}

/**
 *
 */
LengthWeight* Manager::GetLengthWeight(const string& label) const {
  for (auto length_weight : objects_) {
    if (length_weight->label() == label)
      return length_weight;
  }

  return nullptr;
}

} /* namespace lengthweights */
} /* namespace niwa */
