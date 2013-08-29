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
namespace isam {
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
  cout << "Looking for size_weight: " << label << endl;
  for (SizeWeightPtr size_weight : objects_) {
    cout << "Checking size_weight: " << size_weight->label() << endl;
    if (size_weight->label() == label)
      return size_weight;
  }

  cout << "None found" << endl;
  return SizeWeightPtr();
}

} /* namespace sizeweights */
} /* namespace isam */
