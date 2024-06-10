/**
 * @file Simulate.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/05/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "Simulate.h"

#include "../Model/Objects.h"
#include "../Utilities/To.h"

// namespaces
namespace niwa {

/**
 * Default constructor
 */
Simulate::Simulate(shared_ptr<Model> model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "The label for the simulation command block", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "The type of simulation", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years to simulate values for", "", false);
  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "The parameter to generate simulated values for", "");

  original_value_ = 0;
}

/**
 * Validate the objects
 */
void Simulate::Validate() {
  parameters_.Populate(model_);
  DoValidate();
}

/**
 * Build the objects
 */
void Simulate::Build() {
  if (parameter_ == "") {
    parameters().Add(PARAM_PARAMETER, label_, parameters_.Get(PARAM_LABEL)->file_name(), parameters_.Get(PARAM_LABEL)->line_number());
    parameter_ = label_;
  }

  string error = "";
  if (!model_->objects().VerifyAddressableForUse(parameter_, addressable::kSimulate, error)) {
    LOG_FATAL_P(PARAM_PARAMETER) << "could not be verified for use in a simulation. Error: " << error;
  }

  addressable::Type addressable_type = model_->objects().GetAddressableType(parameter_);
  switch (addressable_type) {
    case addressable::kInvalid:
      LOG_ERROR_Q(PARAM_PARAMETER) << error;
      break;
    case addressable::kSingle:
      update_function_ = &Simulate::set_single_value;
      addressable_     = model_->objects().GetAddressable(parameter_);
      original_value_  = *addressable_;
      break;
    case addressable::kVector:
      update_function_    = &Simulate::set_vector_value;
      addressable_vector_ = model_->objects().GetAddressableVector(parameter_);
      break;
    case addressable::kUnsignedMap:
      update_function_ = &Simulate::set_map_value;
      addressable_map_ = model_->objects().GetAddressableUMap(parameter_);
      break;
    default:
      LOG_ERROR() << "The addressable provided for use in a simulation (paramter = " << parameter_ << ") is not a type that is supported in simulations";
      break;
  }
  if (error != "")
    LOG_ERROR_Q(PARAM_PARAMETER) << error;

  DoBuild();
}

/**
 * Update the objects
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
 * Restore the original value of the object
 */
void Simulate::RestoreOriginalValue() {
  LOG_TRACE();
  LOG_FINE() << "Setting original value to: " << AS_DOUBLE(original_value_);
  (this->*update_function_)(original_value_);
}

/**
 * Set the value for an addressable
 *
 * @param value The value
 */
void Simulate::set_single_value(Double value) {
  *addressable_ = value;
}

/**
 * Set the value for an addressable vector
 *
 * @param value The value
 */
void Simulate::set_vector_value(Double value) {
  addressable_vector_->push_back(value);
}

/**
 * Set the value for an addressable map
 *
 * @param value The value
 */
void Simulate::set_map_value(Double value) {
  (*addressable_map_)[model_->current_year()] = value;
}

} /* namespace niwa */
