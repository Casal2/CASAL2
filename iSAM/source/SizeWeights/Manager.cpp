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

// namespaces
namespace niwa {
namespace sizeweights {

/**
 * class definition
 */
Manager::Manager() {
}

/**
 *
 */
SizeWeightPtr Manager::GetSizeWeight(const string& label) const {
  for (SizeWeightPtr size_weight : objects_) {
    if (size_weight->label() == label)
      return size_weight;
  }

  return SizeWeightPtr();
}

} /* namespace sizeweights */
} /* namespace niwa */
