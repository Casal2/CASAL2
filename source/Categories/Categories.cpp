/**
 * @file Categories.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 11/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Categories.h"
#include "Utilities/Logging/Logging.h"

// Namespaces
namespace isam {

/**
 * Default constructor
 */
Categories::Categories() {
  LOG_TRACE();

  parameters_.RegisterAllowed(PARAM_FORMAT);
  parameters_.RegisterAllowed(PARAM_NAMES);
  parameters_.RegisterAllowed(PARAM_YEARS);
  parameters_.RegisterAllowed(PARAM_AGES);
}

/**
 * Our singleton accessor method
 *
 * @return singleton shared ptr
 */
shared_ptr<Categories> Categories::Instance() {
  static CategoriesPtr categories = CategoriesPtr(new Categories());
  return categories;
}

} /* namespace isam */
