/**
 * @file Prior.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 6/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Prior.h"

// Namespaces
namespace isam {

/**
 * Default Constructor
 */
Prior::Prior() {
  parameters_.RegisterAllowed(PARAM_LABEL);
  parameters_.RegisterAllowed(PARAM_TYPE);
}

/**
 * Destructor
 */
Prior::~Prior() {

}

/**
 *
 */
void Prior::Validate() {
  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_TYPE);

  label_ = parameters_.Get(PARAM_LABEL).GetValue<string>();
  type_  = parameters_.Get(PARAM_TYPE).GetValue<string>();
}

} /* namespace isam */
