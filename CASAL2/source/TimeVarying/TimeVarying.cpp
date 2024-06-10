/**
 * @file TimeVarying.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 27/01/2015
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
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

  // Define if we recommended that this be allowed to be estimable
  if (type_ == PARAM_RANDOMDRAW || type_ == PARAM_RANDOMWALK) {
    IsEstimableType_ = false;
  } else {
    IsEstimableType_ = true;
  }
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
    LOG_FATAL_Q(PARAM_PARAMETER) << "the parameter " << parameter_ << " could not be verified for use in an @time_varying block. Error: " << error;
  }

  // Split parameter label up
  // I added this so if the user supplies a string map we can get the string key.
  auto           pair      = model_->objects().ExplodeParameterAndIndex(parameter_);
  string         parameter = pair.first;
  string         index     = pair.second;
  vector<string> indexes;
  if (index != "") {
    indexes = utilities::String::explode(index);
    if (index != "" && indexes.size() == 0) {
      LOG_FATAL_P(PARAM_PARAMETER) << "could not be split up to search for indexes because the format was invalid. "
                                   << "Check the indices. Only the operators ',' and ':' (range) are supported";
    }
  }
  LOG_FINE() << "sizes = " << indexes.size() << " parameter = " << parameter << " index = " << index << " param " << parameter_;

  // bind our function pointer for the update function, original value and addressible pointer
  addressable::Type addressable_type = model_->objects().GetAddressableType(parameter_);
  LOG_FINE() << "addressable type = " << addressable_type;
  switch (addressable_type) {
    case addressable::kInvalid:
      LOG_ERROR_Q(PARAM_PARAMETER) << error;
      break;
    case addressable::kSingle:
      LOG_FINE() << "get time-vary kSingle";
      update_function_ = &TimeVarying::set_single_value;
      addressable_     = model_->objects().GetAddressable(parameter_);
      original_value_  = *addressable_;
      break;
    case addressable::kVector:
      LOG_FINE() << "get time-vary kVector";
      update_function_    = &TimeVarying::set_vector_value;
      addressable_vector_ = model_->objects().GetAddressableVector(parameter_);
      break;
    case addressable::kUnsignedMap:
      LOG_FINE() << "get time-vary kUnsignedMap";
      update_function_ = &TimeVarying::set_map_value;
      addressable_map_ = model_->objects().GetAddressableUMap(parameter_);
      break;
    case addressable::kStringMap:
      LOG_FINE() << "get time-vary kStringMap";
      update_function_ = &TimeVarying::set_string_map_value;
      if (indexes.size() == 0)
        LOG_FATAL_P(PARAM_PARAMETER)
            << "If you supply an addressable that is referenced by a string, you need to supply the index (values within the {}) for the time-vary class to handle it e.g., process[category_transition].proportions{OYS}";
      if (indexes.size() != 1)
        LOG_FATAL_P(PARAM_PARAMETER) << "Can only time-vary string maps with a single index i.e., one category label. The parameter supplied has " << indexes.size();
      string_map_key_        = index;
      addressable_sting_map_ = model_->objects().GetAddressableSMap(parameter_);
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

  if (years_.size() > 0 && std::find(years_.begin(), years_.end(), model_year) == years_.end()) {
    RestoreOriginalValue();
    LOG_FINEST() << "setting value to " << original_value_;
    parameter_by_year_[model_year] = original_value_;
  } else
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
void TimeVarying::set_string_map_value(Double value) {
  (*addressable_sting_map_)[string_map_key_] = value;
}

/**
 * Append a new value to the end of the addressable vector
 * TODO: I don't think this is correct. We need to look at AddressableTransformations for how to deal with this better
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
 * Change the value of the addressable
 *
 * @param value The value to assign to the addressable
 */
void TimeVarying::set_single_value(Double value) {
  *addressable_ = value;
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
/**
 * Verify the TimeVarying object
 * This parameter is not in @estimate
 * This parameter is not in @profile during profile run mode
 * This parameter is not in @parameter_transformation during profile run mode
 */
void TimeVarying::Verify(shared_ptr<Model> model) {
  LOG_FINE() << "Verify parameters useage = " << parameter_;
  if (model->objects().IsParameterUsedFor(parameter_, addressable::kEstimate)) {
    if (!IsEstimableType_)
      LOG_WARNING_Q(PARAM_PARAMETER) << "found an @estimate block for " << parameter_ << " which is of type " << type_
                                     << ". Estimation is not recommended for some time-varying types (e.g., " << PARAM_RANDOMDRAW << "). Please check your configuration file.";
  }
  if (model->objects().IsParameterUsedFor(parameter_, addressable::kTransformation)) {
    LOG_FATAL_Q(PARAM_PARAMETER) << "found an @parameter_transformation block for " << parameter_
                                 << ". You cannot have a time-varying block and a parameter-transformation for the same parameter.";
  }
  if (model->objects().IsParameterUsedFor(parameter_, addressable::kProfile)) {
    LOG_FATAL_Q(PARAM_PARAMETER) << "found an @profile block for " << parameter_ << ". You cannot have a time-varying block and a @profile for the same parameter.";
  }
}
} /* namespace niwa */
