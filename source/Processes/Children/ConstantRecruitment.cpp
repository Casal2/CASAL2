/**
 * @file ConstantRecruitment.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 14/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "ConstantRecruitment.h"

// Namespaces
namespace isam {
namespace processes {

/**
 * Default Constructor
 */
ConstantRecruitment::ConstantRecruitment() {
  parameters_.RegisterAllowed(PARAM_TYPE);
  parameters_.RegisterAllowed(PARAM_CATEGORIES);
  parameters_.RegisterAllowed(PARAM_AGES);
  parameters_.RegisterAllowed(PARAM_R0);
}

/**
 * Destructor
 */
ConstantRecruitment::~ConstantRecruitment() {
}

/**
 * Validate the parameters for this process
 */
void ConstantRecruitment::Validate() {

  // Check for the required parameters
  CheckForRequiredParameter(PARAM_CATEGORIES);
  CheckForRequiredParameter(PARAM_AGES);
  CheckForRequiredParameter(PARAM_R0);
}

/**
 * Build any runtime relationships we might
 * have to other objects in the system.
 */
void ConstantRecruitment::Build() {

}

/**
 *
 */
void ConstantRecruitment::Execute() {

}

} /* namespace processes */
} /* namespace isam */
