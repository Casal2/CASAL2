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
Constant::Constant(shared_ptr<Model> model) : Simulate(model) {
  parameters_.Bind<Double>(PARAM_VALUE, &value_, "The value to assign to the parameter in the simulations", "");
}

/**
 * Validate
 */
void Constant::DoValidate() {}

/**
 * Build
 */
void Constant::DoBuild() {}

/**
 * Reset
 */
void Constant::DoReset() {}

/**
 * Update
 */
void Constant::DoUpdate() {
  //TODO: Implement this
  LOG_FINE() << "Setting Value to: " << value_ << " (Currently Disabled)";
  //  estimable_->SetValueForCurrentYear(value_);
}

} /* namespace simulates */
} /* namespace niwa */
