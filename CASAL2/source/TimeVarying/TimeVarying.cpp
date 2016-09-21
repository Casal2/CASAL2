/**
 * @file TimeVarying.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 27/01/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
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
  target_object_ = model_->objects().FindObject(parameter_, error);

  Estimable::Type estimable_type = model_->objects().GetEstimableType(parameter_, error);
  // Random Walk type can only be associated with an estimable of type ksingle
  if ((estimable_type != Estimable::kSingle && type_ == PARAM_RANDOMWALK) | (estimable_type != Estimable::kSingle && type_ == PARAM_LINEAR))
    LOG_ERROR_P(PARAM_TYPE) << "@time_varying blocks of type " << PARAM_RANDOMWALK << " or " << PARAM_LINEAR << " can only be implemented in parameters that are scalars or single values";

  switch(estimable_type) {
    case Estimable::kInvalid:
      LOG_ERROR_P(PARAM_PARAMETER) << error;
      break;
    case Estimable::kSingle:
      update_function_ = &TimeVarying::set_single_value;
      estimable_    = model_->objects().GetEstimable(parameter_, error);
      if (estimable_ != nullptr) {
        original_value_ = *estimable_;
      }
      break;
    case Estimable::kVector:
      update_function_ = &TimeVarying::set_vector_value;
      estimable_vector_ = model_->objects().GetEstimableVector(parameter_, error);
      break;
    case Estimable::kUnsignedMap:
      update_function_ = &TimeVarying::set_map_value;
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
void TimeVarying::Update(unsigned current_year) {
  LOG_TRACE();
  if (update_function_ == 0)
    LOG_CODE_ERROR() << "DoUpdateFunc_ == 0";

  if (years_.size() > 0 && std::find(years_.begin(), years_.end(), current_year) == years_.end())
    RestoreOriginalValue();
  else
    DoUpdate();

  target_object_->Reset();
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
  *estimable_ = value;
}

/**
 *
 */
void TimeVarying::set_vector_value(Double value) {
  estimable_vector_->push_back(value);
}

/**
 *
 */
void TimeVarying::set_map_value(Double value) {
  (*estimable_map_)[model_->current_year()] = value;
}

/**
 *
 */
void TimeVarying::Reset() {
  string error = "";
  Estimable::Type estimable_type = model_->objects().GetEstimableType(parameter_, error);
  if (estimable_type == Estimable::kSingle) {
    if (estimable_ != nullptr) {
      original_value_ = *estimable_;
    }
  }
}

} /* namespace niwa */
