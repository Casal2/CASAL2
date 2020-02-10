/**
 * @file VectorAverage.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 28/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "VectorAverage.h"

#include "Model/Model.h"
#include "Model/Objects.h"

// namespaces
namespace niwa {
namespace additionalpriors {

/**
 * Default constructor
 */
VectorAverage::VectorAverage(Model* model) : AdditionalPrior(model) {
  parameters_.Bind<string>(PARAM_METHOD, &method_, "Which calculation method to use: k, l, or m", "", PARAM_K);
  parameters_.Bind<Double>(PARAM_K, &k_, "k value to use in the calculation", "");
  parameters_.Bind<double>(PARAM_MULTIPLIER, &multiplier_, "Multiplier for the penalty amount", "", 1);
}

/**
 * Validate our parameters
 */
void VectorAverage::DoValidate() { }

/**
 * Build relationships between this object and other objects
 */
void VectorAverage::DoBuild() {
  string error = "";
  if (!model_->objects().VerfiyAddressableForUse(parameter_, addressable::kLookup, error)) {
    LOG_FATAL_P(PARAM_PARAMETER) << "could not be verified for use in additional_prior.vector_average. Error: " << error;
  }

  addressable::Type addressable_type = model_->objects().GetAddressableType(parameter_);
  LOG_FINEST() << "type = " << addressable_type;
  switch(addressable_type) {
    case addressable::kInvalid:
      LOG_CODE_ERROR() << "Invalid addressable type: " << parameter_;
      break;
    case addressable::kVector:
      addressable_vector_ = model_->objects().GetAddressableVector(parameter_);
      break;
    case addressable::kMultiple:
      addressable_ptr_vector_ = model_->objects().GetAddressables(parameter_);
      break;
    case addressable::kUnsignedMap:
      addressable_map_ = model_->objects().GetAddressableUMap(parameter_);
      break;
    default:
      LOG_ERROR() << "The addressable provided for use in additional priors '" << parameter_
        << "' has a type that is not supported for vector average additional priors";
      break;
  }
}

/**
 * Get the score for this penalty
 *
 * @return Penalty score
 */
Double VectorAverage::GetScore() {
  vector<Double> values;
  if (addressable_vector_ != 0)
    values.assign((*addressable_vector_).begin(), (*addressable_vector_).end());
  else if (addressable_map_ != 0) {
    for (auto iter : (*addressable_map_))
      values.push_back(iter.second);
  } else if (addressable_ptr_vector_ != nullptr) {
    for (auto ptr : (*addressable_ptr_vector_))
      values.push_back((*ptr));
  } else
    LOG_CODE_ERROR() << "(addressable_vector_ != 0) && (addressable_map_ != 0)";

  Double score = 0.0;

  if (method_ == PARAM_K) {
    for (Double value : values)
      score += value;
    score = score / values.size();

    score = (score - k_) * (score - k_);

  } else if (method_ == PARAM_L) {
    for (Double value : values)
      score += log(value);
    score = score / values.size();

    score = (score - k_) * (score - k_);

  } else if (method_ == PARAM_M) {
    for (Double value : values)
      score += value;
    score = log(score / values.size());

    score = (score - k_) * (score - k_);

  } else
    LOG_CODE_ERROR() << "Unknown Method: " << method_;

  return score * multiplier_;
}

} /* namespace additionalpriors */
} /* namespace niwa */
