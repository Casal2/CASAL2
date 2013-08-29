/**
 * @file SizeWeight.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */

// headers
#include "SizeWeight.h"

// namespaces
namespace isam {

/**
 * default constructor
 */
SizeWeight::SizeWeight() {
  parameters_.RegisterAllowed(PARAM_LABEL);
  parameters_.RegisterAllowed(PARAM_TYPE);
}

void SizeWeight::Validate() {
  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_TYPE);

  label_ = parameters_.Get(PARAM_LABEL).GetValue<string>();

  DoValidate();
}

} /* namespace isam */
