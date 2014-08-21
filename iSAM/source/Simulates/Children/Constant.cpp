/**
 * @file Constant.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 8/08/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Constant.h"

// namespaces
namespace isam {
namespace simulates {

/**
 * Default constructor
 */
Constant::Constant() {
  parameters_.Bind<double>(PARAM_VALUE, &value_, "Value to assign to estimable", "");
}

/**
 * Validate
 */
void Constant::DoValidate() { }

/**
 * Build
 */
void Constant::DoBuild() { }

/**
 * Reset
 */
void Constant::DoReset() { }

/**
 *
 */
void Constant::DoUpdate() {
  LOG_INFO("Setting Value to: " << value_);
//  estimable_->SetValueForCurrentYear(value_);
}

} /* namespace simulates */
} /* namespace isam */
