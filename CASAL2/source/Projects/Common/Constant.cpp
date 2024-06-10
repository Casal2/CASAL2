/**
 * @file Constant.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/05/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "Constant.h"

// namespaces
namespace niwa {
namespace projects {

/**
 * Default constructor
 */
Constant::Constant(shared_ptr<Model> model) : Project(model) {
  parameters_.Bind<Double>(PARAM_VALUES, &values_, "The values to assign to the addressable", "");
}

/**
 * Validate
 */
void Constant::DoValidate() {
  if (values_.size() != 1 && values_.size() != years_.size()) {
    LOG_ERROR_P(PARAM_VALUES) << "length (" << values_.size() << ") must match the number of years provided (" << years_.size() << "), or use a single value for all years";
    return;
  }

  if (values_.size() == 1) {
    Double value = values_[0];
    values_.assign(years_.size(), value);
    LOG_FINEST() << "number of values converted from 1 to " << values_.size();
  }
  for (unsigned i = 0; i < years_.size(); ++i) {
    LOG_FINEST() << "value in year " << years_[i] << " = " << values_[i];
    year_values_[years_[i]] = values_[i];
  }
}

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
  value_ = year_values_[model_->current_year()] * multiplier_;
  LOG_FINE() << "Setting Value to: " << value_;
  (this->*DoUpdateFunc_)(value_, true, model_->current_year());
}

} /* namespace projects */
} /* namespace niwa */
