/*
 * Constant.cpp
 *
 *  Created on: 28/01/2015
 *      Author: Admin
 */

#include <TimeVarying/Children/Constant.h>

namespace niwa {
namespace timevarying {

/**
 * Default constructor
 */
Constant::Constant() {
  parameters_.Bind<Double>(PARAM_VALUE, &value_, "Value to assign to estimable", "");
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
  (this->*DoUpdateFunc_)(value_);
}

} /* namespace timevarying */
} /* namespace niwa */
