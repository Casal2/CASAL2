/**
 * @file Catchability.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 12/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Catchability.h"

// Namespaces
namespace isam {

/**
 * Default constructor
 */
Catchability::Catchability() {
  parameters_.RegisterAllowed(PARAM_LABEL, ParameterType::String, "Label");
  parameters_.RegisterAllowed(PARAM_Q, ParameterType::Double, "The catchability amount");

  RegisterAsEstimable(PARAM_Q, &q_);
}

/**
 * Destructor
 */
Catchability::~Catchability() {
}

/**
 * Validate our configuration file parameters
 */
void Catchability::Validate() {
  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_Q);

  label_ = parameters_.Get(PARAM_LABEL).GetValue<string>();
  q_ = parameters_.Get(PARAM_Q).GetValue<double>();
}

} /* namespace isam */
