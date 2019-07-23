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
 * Build our object now.
 *
 * This starts off by
 */
void TimeVarying::Build() {
  // Verify our addressable is allowed to be used for TimeVarying
  string error = "";
  if (!model_->objects().VerfiyAddressableForUse(parameter_, addressable::kTimeVarying, error)) {
    LOG_FATAL_P(PARAM_PARAMETER) << "could not be verified for use in a time_varying block. Error was " << error << ", please double checked you have specified the parameter correctly.";
  }

  // bind our function pointer for the update function, original value and addressible pointer
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

  // Get Target Object variable.
  target_object_ = model_->objects().FindObject(parameter_);

  DoBuild();
}

/**
 * Update target object.
 *
 * Firstly, check if the year passed in is a year we want to change
 * the value for. If it is, then we change value. If not then
 * we restore the original value
 *
 * @param current_year The year we're currently in
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
  target_object_->NotifySubscribers();
}

/**
 * Restore the original value we have to our addressable.
 */
void TimeVarying::RestoreOriginalValue() {
  LOG_TRACE();
  LOG_FINE() << "Setting original value to: " << original_value_;
  (this->*update_function_)(original_value_);
}

/**
 * Change the value of our addressable
 *
 * @param value The value to assign to our addressable
 */
void TimeVarying::set_single_value(Double value) {
  *addressable_ = value;
}

/**
 * Add a new value to the end of our addressable vector.
 *
 * @param value The value to add to our vector
 */
void TimeVarying::set_vector_value(Double value) {
  addressable_vector_->push_back(value);
}

/**
 * Set the value for the current year to the value provided.
 *
 * @param value The value to put into the addressable map
 */
void TimeVarying::set_map_value(Double value) {
  (*addressable_map_)[model_->current_year()] = value;
}

/**
 * Reset our TimeVarying object.
 *
 * During the reset we want to get the original value again
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
