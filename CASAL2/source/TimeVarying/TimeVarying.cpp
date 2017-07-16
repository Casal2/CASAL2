/**
 * @file TimeVarying.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 27/01/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */

// headers
#include "TimeVarying.h"

#include "Model/Objects.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {

/**
 * Default constructor
 */
TimeVarying::TimeVarying(Model* model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "The time-varying label", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "The time-varying type", "", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years in which to vary the values", "");
  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "The name of the parameter to time vary", "");

  original_value_ = 0;
}

/**
 *
 */
void TimeVarying::Validate() {
  parameters_.Populate();
  DoValidate();
}

/**
 *
 */
void TimeVarying::Build() {
  if (parameter_ == "") {
    parameters().Add(PARAM_PARAMETER, label_, parameters_.Get(PARAM_LABEL)->file_name(), parameters_.Get(PARAM_LABEL)->line_number());
    parameter_ = label_;
  }

  string error = "";
  if (!model_->objects().VerfiyAddressableForUse(parameter_, addressable::kTimeVarying, error)) {
    LOG_FATAL_P(PARAM_PARAMETER) << "could not be verified for use in a time_varying block. Error was " << error << ", please double checked you have specified the parameter correctly.";
  }

  addressable::Type addressable_type = model_->objects().GetAddressableType(parameter_);
  switch(addressable_type) {
    case addressable::kInvalid:
      LOG_ERROR_P(PARAM_PARAMETER) << error;
      break;
    case addressable::kSingle:
      update_function_ = &TimeVarying::set_single_value;
      addressable_    = model_->objects().GetAddressable(parameter_);
      original_value_ = *addressable_;
      break;
    case addressable::kVector:
      update_function_ = &TimeVarying::set_vector_value;
      addressable_vector_ = model_->objects().GetAddressableVector(parameter_);
      break;
    case addressable::kUnsignedMap:
      update_function_ = &TimeVarying::set_map_value;
      addressable_map_ = model_->objects().GetAddressableUMap(parameter_);
      break;
    default:
      LOG_ERROR() << "The addressable you have provided for use in a time varying: " << parameter_ << " is not a type that is supported";
      break;
  }

  target_object_ = model_->objects().FindObject(parameter_);

  DoBuild();
}

/**
 * UpDate target object
 */
void TimeVarying::Update(unsigned current_year) {
  LOG_TRACE();
  if (update_function_ == 0)
    LOG_CODE_ERROR() << "DoUpdateFunc_ == 0";

  if (years_.size() > 0 && std::find(years_.begin(), years_.end(), current_year) == years_.end())
    RestoreOriginalValue();
  else
    DoUpdate();

  target_object_->RebuildCache();
}

/**
 *
 */
void TimeVarying::RestoreOriginalValue() {
  LOG_TRACE();
  LOG_FINE() << "Setting original value to: " << original_value_;
  (this->*update_function_)(original_value_);
}

/**
 *
 */
void TimeVarying::set_single_value(Double value) {
  *addressable_ = value;
}

/**
 *
 */
void TimeVarying::set_vector_value(Double value) {
  addressable_vector_->push_back(value);
}

/**
 *
 */
void TimeVarying::set_map_value(Double value) {
  (*addressable_map_)[model_->current_year()] = value;
}

/**
 *
 */
void TimeVarying::Reset() {
  string error = "";
  if (addressable_ != nullptr)
    original_value_ = *addressable_;
}

} /* namespace niwa */
