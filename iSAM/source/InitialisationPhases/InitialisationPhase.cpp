/**
 * @file InitialisationPhase.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "InitialisationPhase.h"

// Namespaces
namespace niwa {

/**
 * Default constructor
 */
InitialisationPhase::InitialisationPhase() {
  LOG_TRACE();

  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type", "", PARAM_ITERATIVE);
}

/**
 * Validate our initialisation phase.
 *
 * 1. Check for any required parameters
 * 2. Assign local variables from parameters
 */
void InitialisationPhase::Validate() {
  parameters_.Populate();
  DoValidate();
}

} /* namespace niwa */
