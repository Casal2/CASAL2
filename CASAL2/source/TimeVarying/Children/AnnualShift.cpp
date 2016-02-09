/**
 * @file AnnualShift.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 2/02/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "AnnualShift.h"

#include "Utilities/Map.h"

// namespaces
namespace niwa {
namespace timevarying {

/**
 * Default constructor
 */
AnnualShift::AnnualShift(Model* model) : TimeVarying(model) {
  parameters_.Bind<Double>(PARAM_VALUES, &values_, "", "");
  parameters_.Bind<Double>(PARAM_A, &a_, "", "");
  parameters_.Bind<Double>(PARAM_B, &b_, "", "");
  parameters_.Bind<Double>(PARAM_C, &c_, "", "");
  parameters_.Bind<unsigned>(PARAM_SCALING_YEARS, &scaling_years_, "" ,"", true);

}

/**
 *
 */
void AnnualShift::DoValidate() {
  if (years_.size() != values_.size())
    LOG_ERROR_P(PARAM_YEARS) << " provided (" << years_.size() << ") does not match the number of values provided (" << values_.size() << ")";

  if (scaling_years_.size() == 0)
    scaling_years_ = years_;

  for (unsigned year : scaling_years_) {
    if (std::find(years_.begin(), years_.end(), year) == years_.end())
      LOG_ERROR_P(PARAM_SCALING_YEARS) << " value (" << year << ") has not been defined as one of the years";
  }
}

/**
 *
 */
void AnnualShift::DoBuild() {
  map<unsigned, Double> values = utilities::Map::create(years_, values_);

  Double total = 0.0;
  for (unsigned scaling_year : scaling_years_) {
    total += values[scaling_year];
  }

  for (unsigned year : years_) {
    Double scaled_value = values[year] - (total / scaling_years_.size());
    values_by_year_[year] = a_ * scaled_value + b_ * pow(scaled_value, 2) + c_ * pow(scaled_value, 3);
  }
}

/**
 *
 */
void AnnualShift::DoReset() {

}

/**
 *
 */
void AnnualShift::DoUpdate() {
  LOG_FINE() << "Setting Value to: " << values_by_year_[model_->current_year()];
  (this->*update_function_)(values_by_year_[model_->current_year()]);
}

} /* namespace timevarying */
} /* namespace niwa */
