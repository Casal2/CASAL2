/**
 * @file Simulate.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/05/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Simulate.h"

#include "Model/Objects.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {

/**
 * Default constructor
 */
Simulate::Simulate(Model* model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type", "", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years to recalculate the values", "", true);
  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "Parameter to Simulate", "");

  original_value_ = 0;
}

/**
 *
 */
void Simulate::Validate() {
  parameters_.Populate();
  DoValidate();
}

/**
 *
 */
void Simulate::Build() {
  if (parameter_ == "") {
    parameters().Add(PARAM_PARAMETER, label_, parameters_.Get(PARAM_LABEL)->file_name(), parameters_.Get(PARAM_LABEL)->line_number());
    parameter_ = label_;
  }

  string error = "";
  Estimable::Type estimable_type = model_->objects().GetEstimableType(parameter_, error);
  switch(estimable_type) {
    case Estimable::kInvalid:
      LOG_ERROR_P(PARAM_PARAMETER) << error;
      break;
    case Estimable::kSingle:
      update_function_ = &Simulate::set_single_value;
      estimable_    = model_->objects().GetEstimable(parameter_, error);
      original_value_ = *estimable_;
      break;
    case Estimable::kVector:
      update_function_ = &Simulate::set_vector_value;
      estimable_vector_ = model_->objects().GetEstimableVector(parameter_, error);
      break;
    case Estimable::kUnsignedMap:
      update_function_ = &Simulate::set_map_value;
      estimable_map_ = model_->objects().GetEstimableUMap(parameter_, error);
      break;
    default:
      LOG_ERROR() << "The estimable you have provided for use in a time varying: " << parameter_ << " is not a type that is supported";
      break;
  }
  if (error != "")
    LOG_ERROR_P(PARAM_PARAMETER) << error;

  DoBuild();
}

/**
 *
 */
void Simulate::Update(unsigned current_year) {
  LOG_TRACE();
  if (update_function_ == 0)
    LOG_CODE_ERROR() << "update_function_ == 0";

  if (years_.size() > 0 && std::find(years_.begin(), years_.end(), current_year) == years_.end())
    RestoreOriginalValue();
  else
    DoUpdate();
}

/**
 *
 */
void Simulate::RestoreOriginalValue() {
  LOG_TRACE();
  LOG_FINE() << "Setting original value to: " << AS_DOUBLE(original_value_);
  (this->*update_function_)(original_value_);
}

/**
 *
 */
void Simulate::set_single_value(Double value) {
  *estimable_ = value;
}

/**
 *
 */
void Simulate::set_vector_value(Double value) {
  estimable_vector_->push_back(value);
}

/**
 *
 */
void Simulate::set_map_value(Double value) {
  (*estimable_map_)[model_->current_year()] = value;
}

} /* namespace niwa */
