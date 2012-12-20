/**
 * @file Ageing.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 20/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Ageing.h"

#include "Utilities/To.h"

// Namespaces
namespace isam {
namespace processes {

/**
 * Default constructor
 */
Ageing::Ageing() {
  LOG_TRACE();

  parameters_.RegisterAllowed(PARAM_LABEL);
  parameters_.RegisterAllowed(PARAM_CATEGORIES);
}

/**
 * Validate our ageing process.
 *
 * 1. Check for any required parameters
 * 2. Assign the label from our parameters
 * 3. Assign any remaining parameters
 */
void Ageing::Validate() {

  CheckForRequiredParameter(PARAM_CATEGORIES);
  AssignLabelFromParameters();

  category_names_ = parameters_.Get(PARAM_CATEGORIES).GetValues<string>();
}

/**
 *
 */
void Ageing::Build() {

}

/**
 *
 */
void Ageing::Execute() {

}

} /* namespace processes */
} /* namespace isam */
