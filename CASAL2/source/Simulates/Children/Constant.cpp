/**
 * @file Constant.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 8/08/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */

// headers
#include "Constant.h"

// namespaces
namespace niwa {
namespace simulates {

/**
 * Default constructor
 */
Constant::Constant(Model* model) : Simulate(model) {
  parameters_.Bind<Double>(PARAM_VALUE, &value_, "Value to assign to addressable", "");
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
  LOG_FINE() << "Setting Value to: " << value_ << " (Currently Disabled)";
//  estimable_->SetValueForCurrentYear(value_);
}

} /* namespace simulates */
} /* namespace niwa */
