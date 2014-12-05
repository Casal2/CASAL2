/**
 * @file Manager.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 12/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Manager.h"

// Namespaces
namespace niwa {
namespace catchabilities {

/**
 * Default constructor
 */
Manager::Manager() {
}

/**
 * Destructor
 */
Manager::~Manager() noexcept(true) {
}

/**
 * Find and return pointer to catchability
 *
 * @param label The label of the catchability
 * @return Catchability pointer
 */
CatchabilityPtr Manager::GetCatchability(const string& label) {
  CatchabilityPtr result;

  for(CatchabilityPtr catchability : objects_) {
    if (catchability->label() == label) {
      result = catchability;
      break;
    }
  }

  return result;
}

} /* namespace catchabilities */
} /* namespace niwa */
