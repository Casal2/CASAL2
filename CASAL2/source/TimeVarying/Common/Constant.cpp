/*
 * Constant.cpp
 *
 *  Created on: 28/01/2015
 *      Author: Admin
 */

#include "Constant.h"

namespace niwa {
namespace timevarying {

/**
 * Default constructor
 */
Constant::Constant(Model* model) : TimeVarying(model) {
  parameters_.Bind<Double>(PARAM_VALUES, &values_, "Value to assign to addressable", "");

  RegisterAsAddressable(PARAM_VALUES, &parameter_by_year_);
}

/**
 * Validate
 */
void Constant::DoValidate() {
  if (values_.size() != 1 && values_.size() != years_.size()) {
    LOG_ERROR_P(PARAM_VALUES) << "length (" << values_.size() << ") must match the number of years provided (" << years_.size() << ")";
    return;
  }

  if (values_.size() == 1)
    values_.assign(years_.size(), values_[0]);
  for (unsigned i = 0; i < years_.size(); ++i) {
    parameter_by_year_[years_[i]] = values_[i];
  }
}

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
  LOG_FINE() << "Setting Value to: " << parameter_by_year_[model_->current_year()];
  (this->*update_function_)(parameter_by_year_[model_->current_year()]);
}

} /* namespace timevarying */
} /* namespace niwa */
