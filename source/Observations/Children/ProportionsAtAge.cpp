/**
 * @file ProportionsAtAge.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 8/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "ProportionsAtAge.h"

// Namespaces
namespace isam {
namespace observations {

/**
 * Default constructor
 */
ProportionsAtAge::ProportionsAtAge() {
  parameters_.RegisterAllowed(PARAM_MIN_AGE);
  parameters_.RegisterAllowed(PARAM_MAX_AGE);
  parameters_.RegisterAllowed(PARAM_TOLERANCE);
  parameters_.RegisterAllowed(PARAM_OBS);
  parameters_.RegisterAllowed(PARAM_ERROR_VALUE);
  parameters_.RegisterAllowed(PARAM_DELTA);
  parameters_.RegisterAllowed(PARAM_PROCESS_ERROR);
  parameters_.RegisterAllowed(PARAM_AGEING_ERROR);
}

/**
 * Validate configuration file parameters
 */
void ProportionsAtAge::Validate() {
  Observation::Validate();
}

void ProportionsAtAge::Build() {

}

void ProportionsAtAge::PreExecute() {

}

void ProportionsAtAge::Execute() {

}

} /* namespace observations */
} /* namespace isam */
