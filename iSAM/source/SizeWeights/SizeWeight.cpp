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
namespace niwa {

/**
 * default constructor
 */
SizeWeight::SizeWeight() {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type", "");
}

/**
 *
 */
void SizeWeight::Validate() {
  parameters_.Populate();
  DoValidate();
}

} /* namespace niwa */
