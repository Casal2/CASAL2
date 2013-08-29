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
namespace isam {
namespace agesizes {

/**
 *
 */
AgeSizePtr Manager::GetAgeSize(const string& label) {
  for (AgeSizePtr age_size : objects_) {
    if (age_size->label() == label)
      return age_size;
  }

  return AgeSizePtr();
}

} /* namespace agesizes */
} /* namespace isam */
