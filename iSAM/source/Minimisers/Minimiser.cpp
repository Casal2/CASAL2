/**
 * @file Minimiser.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 28/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Minimiser.h"

// Namespaces
namespace isam {

/**
 * Default constructor
 */
Minimiser::Minimiser() {
  parameters_.RegisterAllowed(PARAM_LABEL);
  parameters_.RegisterAllowed(PARAM_TYPE);
  parameters_.RegisterAllowed(PARAM_ACTIVE);
  parameters_.RegisterAllowed(PARAM_COVARIANCE);
}

/**
 * Destructor
 */
Minimiser::~Minimiser() {
}

/**
 * Validate the parameters for this minimisers
 */
void Minimiser::Validate() {
  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_TYPE);

  label_            = parameters_.Get(PARAM_LABEL).GetValue<string>();
  type_             = parameters_.Get(PARAM_TYPE).GetValue<string>();
  active_           = parameters_.Get(PARAM_ACTIVE).GetValue<bool>(false);
  build_covariance_ = parameters_.Get(PARAM_COVARIANCE).GetValue<bool>(true);
}

/**
 * Build the parameters for this minimiser
 */
void Minimiser::Build() {

}

} /* namespace isam */
