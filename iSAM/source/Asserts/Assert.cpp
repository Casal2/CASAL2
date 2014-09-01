/**
 * @file Assert.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 19/08/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#include "Assert.h"

namespace isam {

/**
 *
 */
Assert::Assert() {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label for the assert", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type of the assert", "");
}

/**
 *
 */
void Assert::Validate() {
  parameters_.Populate();
  DoValidate();
}

} /* namespace isam */
