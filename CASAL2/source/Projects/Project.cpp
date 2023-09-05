/**
 * @file Project.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/05/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */

// headers
#include "Project.h"

#include "../Model/Objects.h"
#include "../Utilities/To.h"

// namespaces
namespace niwa {

/**
 * Default constructor
 */
Project::Project(shared_ptr<Model> model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type", "", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years to recalculate the values", "", false);
  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "Parameter to project", "");
  parameters_.Bind<Double>(PARAM_MULTIPLIER, &multiplier_, "Multiplier that is applied to the projected value", "", 1.0)->set_lower_bound(0, false);

  original_value_ = 0;
}

/**
 * Validate the objects
 */
void Project::Validate() {
  parameters_.Populate(model_);
  DoValidate();
}

/**
 * Build the objects
 */
void Project::Build() {
  string error = "";
  if (!model_->objects().VerifyAddressableForUse(parameter_, addressable::kProject, error)) {
    LOG_FATAL_P(PARAM_PARAMETER) << "could not be verified for use in a @project block. Error: " << error;
  }

  addressable::Type addressable_type = model_->objects().GetAddressableType(parameter_);
  switch (addressable_type) {
    case addressable::kInvalid:
      LOG_CODE_ERROR() << "Invalid addressable type: " << parameter_;
      break;
    case addressable::kSingle:
      LOG_FINEST() << "applying projection for parameter " << parameter_ << " is a single value";
      DoUpdateFunc_   = &Project::SetSingleValue;
      addressable_    = model_->objects().GetAddressable(parameter_);
      original_value_ = *addressable_;
      break;
    case addressable::kVector:
      LOG_FINEST() << "applying projection for parameter " << parameter_ << " is a vector";
      addressable_vector_ = model_->objects().GetAddressableVector(parameter_);
      DoUpdateFunc_       = &Project::SetVectorValue;
      break;
    case addressable::kUnsignedMap:
      LOG_FINEST() << "applying projection for parameter " << parameter_ << " is an unsigned map";
      DoUpdateFunc_    = &Project::SetMapValue;
      addressable_map_ = model_->objects().GetAddressableUMap(parameter_);
      break;
    default:
      LOG_ERROR() << "The addressable provided for use in a projection: " << parameter_ << " is not a type that is supported for projection modification";
      break;
  }
  DoBuild();
}

/**
 * Reset and re build any pointers
 */
void Project::Reset() {
  if (addressable_ != nullptr) {
    original_value_ = *addressable_;
  }

  if (model_->projection_final_phase())
    DoReset();
}

/**
 * Update the objects
 */
void Project::Update(unsigned current_year) {
  LOG_TRACE();
  if (DoUpdateFunc_ == nullptr)
    LOG_CODE_ERROR() << "DoUpdateFunc_ == nullptr";
  if (std::find(years_.begin(), years_.end(), current_year) == years_.end()) {
    LOG_FINEST() << "Resetting parameter to original value as the year " << current_year << " is not in years";
    RestoreOriginalValue(current_year);
  } else {
    LOG_FINEST() << "updating parameter";
    DoUpdate();
  }
}

/**
 * Restore the objects to their original value in a specified year
 *
 * @param year The year
 */
void Project::RestoreOriginalValue(unsigned year) {
  LOG_FINE() << "Setting value to: " << stored_values_[year];
  (this->*DoUpdateFunc_)(stored_values_[year], true, year);
}

/**
 * Set an addressable to a specified value
 *
 * @param value The value
 */
void Project::SetSingleValue(Double value, bool save_value, unsigned year) {
  LOG_TRACE();
  *addressable_ = value;
  if (save_value) {
    projected_values_[year] = value;
    std::cerr << "year: " << year << " " << value << "\n";
  }
}

/**
 * Set an addressable vector to a specified value
 *
 * @param value The value
 */
void Project::SetVectorValue(Double value, bool save_value, unsigned year) {
  LOG_FINEST() << "size before adding another value = " << addressable_vector_->size();
  addressable_vector_->push_back(value);
  if (save_value) {
    projected_values_[year] = value;
    std::cerr << "year: " << year << " " << value << "\n";
  }
  LOG_FINEST() << "size before adding a value of " << value << " = " << addressable_vector_->size();
}

/**
 * Set an addressable map to a specified value
 *
 * @param value The value
 */
void Project::SetMapValue(Double value, bool save_value, unsigned year) {
  LOG_TRACE();
  (*addressable_map_)[year] = value;
  if (save_value)
    projected_values_[year] = value;
}

/**
 * Store the value from the addressable for a specified year
 *
 * @param year The year
 */
void Project::StoreValue(unsigned current_year) {
  if (addressable_ != nullptr)
    stored_values_[current_year] = *addressable_;
  else if (addressable_map_ != nullptr)
    stored_values_[current_year] = (*addressable_map_)[current_year];
  else if (addressable_vector_ != nullptr) {
    unsigned index = current_year - model_->start_year();
    if (index >= addressable_vector_->size()) {
      LOG_CODE_ERROR() << "Could not store value for @project parameter " << parameter_ << " in year " << current_year << " because index exceeded size of vector " << index
                       << " : " << addressable_vector_->size();
    }
    stored_values_[current_year] = addressable_vector_->at(index);
  }
  LOG_FINEST() << "Storing value = " << stored_values_[current_year];
}

/*
 * Verify that there is not @time_varying block as well on this parameter
 */
void Project::Verify(shared_ptr<Model> model) {
  // you cant project a time-varying parameter at the moment.
}
/*
 * Verify that there is not @time_varying block as well on this parameter
 */
void Project::SetObjectForNextIteration() {
  LOG_FINE() << "Setting value to: " << stored_values_[model_->start_year()];
  (this->*DoUpdateFunc_)(stored_values_[model_->start_year()], false, model_->start_year());
}

} /* namespace niwa */
