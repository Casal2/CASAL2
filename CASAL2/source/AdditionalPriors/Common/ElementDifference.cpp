/**
 * @file ElementDifference.cpp
 * @author C.Marsh
 * @github https://github.com/Zaita
 * @date 5/7/2017
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 *
 */

// headers
#include "ElementDifference.h"

#include "Estimates/Manager.h"
#include "Model/Model.h"
#include "Model/Objects.h"

// namespaces
namespace niwa {
namespace additionalpriors {

/**
 * Default constructor
 */
ElementDifference::ElementDifference(Model* model) : AdditionalPrior(model) {
  parameters_.Bind<string>(PARAM_SECOND_PARAMETER, &second_parameter_, "Name of the second parameter for comparing", "");
  parameters_.Bind<double>(PARAM_MULTIPLIER, &multiplier_, "Multiply the penalty by this factor", "", 1);
}

/**
 * Validate our parameters
 */
void ElementDifference::DoValidate() {

}

/**
 * Build our parameters
 */
void ElementDifference::DoBuild() {
  LOG_TRACE();
  string error = "";
  if (!model_->objects().VerfiyAddressableForUse(second_parameter_, addressable::kLookup, error)) {
    LOG_FATAL_P(PARAM_SECOND_PARAMETER) << "could not be verified for use in additional_prior.element_difference. Error was " << error;
  }
  error = "";
  if (!model_->objects().VerfiyAddressableForUse(parameter_, addressable::kLookup, error)) {
    LOG_FATAL_P(PARAM_PARAMETER) << "could not be verified for use in additional_prior.element_difference. Error was " << error;
  }
  // first parameter
  addressable::Type addressable_type = model_->objects().GetAddressableType(parameter_);
  LOG_FINEST() << "addressable type = " << addressable_type;
  switch(addressable_type) {
    case addressable::kInvalid:
      LOG_CODE_ERROR() << "Invalid addressable type: " << parameter_;
      break;
    case addressable::kMultiple:
      addressable_ptr_vector_ = model_->objects().GetAddressables(parameter_);
      break;
    case addressable::kVector:
      addressable_vector_ = model_->objects().GetAddressableVector(parameter_);
      break;
    case addressable::kUnsignedMap:
      addressable_map_ = model_->objects().GetAddressableUMap(parameter_);
      break;
    case addressable::kSingle:
      addressable_ = model_->objects().GetAddressable(parameter_);
      break;
    default:
      LOG_ERROR() << "The addressable you have provided for use in a additional priors: " << parameter_
        << " is not a type that is supported for vector smoothing additional priors";
      break;
  }
  // Get second parameter estimates
  addressable_type = model_->objects().GetAddressableType(second_parameter_);
  LOG_FINEST() << "addressable type = " << addressable_type;
  switch(addressable_type) {
    case addressable::kInvalid:
      LOG_CODE_ERROR() << "Invalid addressable type: " << second_parameter_;
      break;
    case addressable::kMultiple:
      second_addressable_ptr_vector_ = model_->objects().GetAddressables(second_parameter_);
      break;
    case addressable::kVector:
      second_addressable_vector_ = model_->objects().GetAddressableVector(second_parameter_);
      break;
    case addressable::kUnsignedMap:
      second_addressable_map_ = model_->objects().GetAddressableUMap(second_parameter_);
      break;
    case addressable::kSingle:
      second_addressable_ = model_->objects().GetAddressable(second_parameter_);
      break;
    default:
      LOG_ERROR() << "The addressable you have provided for use in a additional priors: " << second_parameter_
        << " is not a type that is supported for difference element additional priors";
      break;
  }

  // Check the two parameters are the same length
  vector<Double> values;
  vector<Double> second_values;
  // Load first parameter
  if (addressable_vector_ != nullptr)
    values.assign((*addressable_vector_).begin(), (*addressable_vector_).end());
  else if (addressable_ptr_vector_ != nullptr) {
    for (auto ptr : (*addressable_ptr_vector_))
      values.push_back((*ptr));
  } else if (addressable_map_ != nullptr) {
    for (auto iter : (*addressable_map_))
      values.push_back(iter.second);
  } else if (addressable_ != nullptr) {
    values.push_back((*addressable_));
  } else
    LOG_CODE_ERROR() << "(addressable_map_ != 0) && (addressable_vector_ != 0)";
  // Load second parameter
  if (second_addressable_vector_ != nullptr)
    second_values.assign((*second_addressable_vector_).begin(), (*second_addressable_vector_).end());
  else if (second_addressable_ptr_vector_ != nullptr) {
    for (auto ptr : (*second_addressable_ptr_vector_))
      second_values.push_back((*ptr));
  } else if (second_addressable_map_ != nullptr) {
    for (auto iter : (*second_addressable_map_))
      second_values.push_back(iter.second);
  } else if (second_addressable_ != nullptr) {
    second_values.push_back((*second_addressable_));
  } else
    LOG_CODE_ERROR() << "(second_addressable_map_ != 0) && (second_addressable_vector_ != 0) && (second_addressable_ != 0)";

  if(second_values.size() != values.size())
    LOG_ERROR_P(PARAM_SECOND_PARAMETER) << "The parameters are not the same size, which they need to be, the second parameter has " << second_values.size() << " elements where as, the first parameter has " << values.size() << " elements";

}

/**
 * Get the score for this penalty
 *
 * @return Penalty score
 */
Double ElementDifference::GetScore() {
  LOG_TRACE();
  vector<Double> values;
  vector<Double> second_values;
  // first parameter
  if (addressable_vector_ != nullptr)
    values.assign((*addressable_vector_).begin(), (*addressable_vector_).end());
  else if (addressable_ptr_vector_ != nullptr) {
    for (auto ptr : (*addressable_ptr_vector_))
      values.push_back((*ptr));
  } else if (addressable_map_ != nullptr) {
    for (auto iter : (*addressable_map_))
      values.push_back(iter.second);
  } else if (addressable_ != nullptr) {
    values.push_back((*addressable_));
  } else
    LOG_CODE_ERROR() << "(second_addressable_map_ != 0) && (second_addressable_vector_ != 0)";
  // Second parameter
  if (second_addressable_vector_ != nullptr)
    second_values.assign((*second_addressable_vector_).begin(), (*second_addressable_vector_).end());
  else if (second_addressable_ptr_vector_ != nullptr) {
    for (auto ptr : (*second_addressable_ptr_vector_))
      second_values.push_back((*ptr));
  } else if (second_addressable_map_ != nullptr) {
    for (auto iter : (*second_addressable_map_))
      second_values.push_back(iter.second);
  } else if (second_addressable_ != nullptr) {
      second_values.push_back((*second_addressable_));
  } else
    LOG_CODE_ERROR() << "(second_addressable_map_ != 0) && (second_addressable_vector_ != 0)";

  Double score = 0.0;
  LOG_FINEST() << "size of first vector = " << values.size() << " size of second vector";
  for(unsigned i = 0; i < values.size(); ++i)
    score += pow(values[i] - second_values[i], 2);
  return score * multiplier_;
}

} /* namespace additionalpriors */
} /* namespace niwa */
