/**
 * @file Manager.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 16/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// headers
#include "Manager.h"

// namespaces
namespace niwa {
namespace ageingerrors {

/**
 * Return an AgeingError smart_ptr based on label
 *
 * @param label The label of the age length object
 * @return ageing error pointer
 */
AgeingError* Manager::GetAgeingError(const string& label) {
  for (auto age_error : objects_) {
    if (age_error->label() == label)
      return age_error;
  }

  return nullptr;
}

} /* namespace ageingerrors */
} /* namespace niwa */
