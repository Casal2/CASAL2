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

#include "../Model/Objects.h"
#include "../Utilities/To.h"

// namespaces
namespace niwa {

/**
 * Default constructor
 */
TimeVarying::TimeVarying(shared_ptr<Model> model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "The label of the time-varying object", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "The type of the time-varying object", "", "")
      ->set_allowed_values({PARAM_ANNUAL_SHIFT, PARAM_CONSTANT, PARAM_EXOGENOUS, PARAM_LINEAR, PARAM_RANDOMWALK, PARAM_RANDOMDRAW});
  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "The name of the parameter to vary in each year", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years in which to vary the paramter", "");
}

/**
 * Bind the parameters from the configuration file to this object,
 * then call DoValidate() on the child class
 */
void TimeVarying::Validate() {
  parameters_.Populate(model_);
  DoValidate();

  // set a default parameter that is the label. This makes it
  // easier to define this object in the configuration file
  if (parameter_ == "") {
    parameters().Add(PARAM_PARAMETER, label_, parameters_.Get(PARAM_LABEL)->file_name(), parameters_.Get(PARAM_LABEL)->line_number());
    parameter_ = label_;
  }
}

/**
 * Build the TimeVarying object
 */
void TimeVarying::Build() {
  // Verify our addressable is allowed to be used for TimeVarying
  string error = "";
  if (!model_->objects().VerifyAddressableForUse(parameter_, addressable::kTimeVarying, error)) {
    LOG_FATAL_P(PARAM_PARAMETER) << "could not be verified for use in an @time_varying block. Error: " << error;
  }

  // bind our function pointer for the update function, original value and addressible pointer
  addressable::Type addressable_type = model_->objects().GetAddressableType(parameter_);
  switch (addressable_type) {
    case addressable::kInvalid:
      LOG_ERROR_P(PARAM_PARAMETER) << error;
      break;
    case addressable::kSingle:
      update_function_ = &TimeVarying::set_single_value;
      addressable_     = model_->objects().GetAddressable(parameter_);
      original_value_  = *addressable_;
      break;
    case addressable::kVector:
      update_function_    = &TimeVarying::set_vector_value;
      addressable_vector_ = model_->objects().GetAddressableVector(parameter_);
      break;
    case addressable::kUnsignedMap:
      update_function_ = &TimeVarying::set_map_value;
      addressable_map_ = model_->objects().GetAddressableUMap(parameter_);
      break;
    default:
      LOG_ERROR() << "The addressable provided for use in the @time_varying block: " << parameter_ << " is not a parameter of a type that is supported";
      break;
  }

  // Get Target Object variable.
  target_object_ = model_->objects().FindObject(parameter_);

  DoBuild();
}

/**
 * Update the TimeVarying object
 *
 * First, check if the year passed in is a year to change
 * the value for. If it is, then change the value. If not, then
 * restore the original value
 *
 * @param model_year The model year
 */
void TimeVarying::Update(unsigned model_year) {
  LOG_TRACE();
  if (update_function_ == 0)
    LOG_CODE_ERROR() << "DoUpdateFunc_ == 0";

  if (years_.size() > 0 && std::find(years_.begin(), years_.end(), model_year) == years_.end())
    RestoreOriginalValue();
  else
    DoUpdate();

  target_object_->RebuildCache();
  target_object_->NotifySubscribers();
}

/**
 * Restore the original value of the addressable.
 */
void TimeVarying::RestoreOriginalValue() {
  LOG_TRACE();
  LOG_FINE() << "Setting original value to: " << original_value_;
  (this->*update_function_)(original_value_);
}

/**
 * Change the value of the addressable
 *
 * @param value The value to assign to the addressable
 */
void TimeVarying::set_single_value(Double value) {
  *addressable_ = value;
}

/**
 * Append a new value to the end of the addressable vector
 *
 * @param value The value to add to the addressable vector
 */
void TimeVarying::set_vector_value(Double value) {
  addressable_vector_->push_back(value);
}

/**
 * Set the value for the current year to the value provided
 *
 * @param value The value to put into the addressable map
 */
void TimeVarying::set_map_value(Double value) {
  (*addressable_map_)[model_->current_year()] = value;
}

/**
 * Reset the TimeVarying object
 *
 * During the reset retrieve the original value again
 * because it may change between iterations. This will occur
 * if the object is an addressable that is being estimated
 * or modified as part of a MCMC etc.
 */
void TimeVarying::Reset() {
  string error = "";
  if (addressable_ != nullptr)
    original_value_ = *addressable_;
  DoReset();
}

} /* namespace niwa */
