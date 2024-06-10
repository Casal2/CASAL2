/**
 * @file Exogenous.cpp
 * @author C.Marsh
 * @github https://github.com/Zaita
 * @date 29/09/2015
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "Exogenous.h"

#include "../../Model/Objects.h"
#include "../../Utilities/Map.h"

// namespaces
namespace niwa {
namespace timevarying {

/**
 * Default constructor
 */
Exogenous::Exogenous(shared_ptr<Model> model) : TimeVarying(model) {
  parameters_.Bind<Double>(PARAM_A, &a_, "The shift parameter", "");
  parameters_.Bind<Double>(PARAM_EXOGENOUS_VARIABLE, &exogenous_, "The values of exogenous variable for each year", "");

  RegisterAsAddressable(PARAM_A, &a_);
  RegisterAsAddressable(PARAM_EXOGENOUS_VARIABLE, &exogenous_);
}

/**
 * Validate parameters from config file
 */
void Exogenous::DoValidate() {
  if (years_.size() != exogenous_.size())
    LOG_ERROR_P(PARAM_YEARS) << "provided (" << years_.size() << ") does not match the number of values provided (" << exogenous_.size() << ")";
}

/**
 * Calculate mean of exogenous variable, then using the shift paramter. Store the parameters in the map
 * values_by_year.
 */
void Exogenous::DoBuild() {
  // Check that the parameter is of type scalar
  if (model_->objects().GetAddressableType(parameter_) != addressable::kSingle)
    LOG_ERROR_Q(PARAM_PARAMETER) << ": parameter '" << parameter_ << "'must be a scalar. Other addressable types not supported";
  // calculate mean;
  Double total = 0.0;
  for (Double value : exogenous_) total += value;
  mean_value_ = total / exogenous_.size();
  LOG_FINE() << "mean = " << mean_value_;
  // create values
  values_by_year_ = utilities::Map::create(years_, exogenous_);
  DoReset();
}

/**
 * Reset
 */
void Exogenous::DoReset() {
  // Add this to the Reset so that if a, is estimated the model can actually update the model.

  Double* value = model_->objects().GetAddressable(parameter_);
  LOG_FINEST() << " Parameter value = " << (*value) << " a " << a_;
  for (unsigned year : years_) {
    parameter_by_year_[year] = (*value) + (a_ * (values_by_year_[year] - mean_value_));
    LOG_FINEST() << " year = " << year << " " << values_by_year_[year] << " a_ " << a_ << " mean value = " << mean_value_ << " new param = " << parameter_by_year_[year];
  }
}

/**
 * Update
 */
void Exogenous::DoUpdate() {
  LOG_FINE() << "Setting Value to: " << parameter_by_year_[model_->current_year()];
  (this->*update_function_)(parameter_by_year_[model_->current_year()]);
}

} /* namespace timevarying */
} /* namespace niwa */
