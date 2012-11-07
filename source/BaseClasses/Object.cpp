/**
 * @file BaseObject.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Object.h"

namespace isam {
namespace base {
/**
 * Default Constructor
 */
Object::Object() {

  // Default our members
  label_          = "";
  global_config_  = GlobalConfiguration::Instance();
}

/**
 * Destructor
 */
Object::~Object() {
}

} /* namespace base */
} /* namespace isam */
