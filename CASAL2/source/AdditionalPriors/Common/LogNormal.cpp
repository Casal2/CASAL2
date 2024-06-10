/**
 * @file LogNormal.cpp
 * @author C.Marsh
 * @github https://github.com/Zaita
 * @date 3/7/2017
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "LogNormal.h"

#include "../../Model/Model.h"
#include "../../Model/Objects.h"

// namespaces
namespace niwa {
namespace additionalpriors {

/**
 * Default constructor
 *
 * Bind any parameters that are allowed to be loaded from the configuration files.
 * Set bounds on registered parameters
 * Register any parameters that can be an estimated or utilised in other run modes (e.g., profiling, yields, projections, etc.)
 * Set some initial values
 *
 * Note: The constructor is parsed to generate LaTeX for the documentation.
 */
LogNormal::LogNormal(shared_ptr<Model> model) : AdditionalPrior(model) {
  parameters_.Bind<Double>(PARAM_MU, &mu_, "The lognormal prior mean (mu) parameter", "")->set_lower_bound(0.0, false);
  parameters_.Bind<Double>(PARAM_CV, &cv_, "The lognormal CV parameter", "")->set_lower_bound(0.0, false);
}

/**
 * Populate any parameters,
 * Validate that values are within expected ranges when bind<>() overloads cannot be used
 *
 * Note: all parameters are populated from configuration files
 */
void LogNormal::DoValidate() {}

/**
 * Build the object
 */
void LogNormal::DoBuild() {
  string error = "";
  if (!model_->objects().VerifyAddressableForUse(parameter_, addressable::kLookup, error)) {
    LOG_FATAL_P(PARAM_PARAMETER) << "could not be found. Error: " << error;
  }

  addressable::Type addressable_type = model_->objects().GetAddressableType(parameter_);
  LOG_FINEST() << "type = " << addressable_type;
  switch (addressable_type) {
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
      LOG_ERROR() << "The addressable provided '" << parameter_ << "' has a type that is not supported for lognormal additional priors";
      break;
  }
}

/**
 * Get the score
 * @return The score
 */
Double LogNormal::GetScore() {
  score_ = 0.0;
  vector<Double> values;
  vector<Double> second_values;
  // Load first parameter
  if (addressable_vector_ != nullptr)
    values.assign((*addressable_vector_).begin(), (*addressable_vector_).end());
  else if (addressable_ptr_vector_ != nullptr) {
    for (auto ptr : (*addressable_ptr_vector_)) values.push_back((*ptr));
  } else if (addressable_map_ != nullptr) {
    for (auto iter : (*addressable_map_)) values.push_back(iter.second);
  } else if (addressable_ != nullptr) {
    values.push_back((*addressable_));
  } else {
    LOG_CODE_ERROR() << "(addressable_map_ != 0) && (addressable_vector_ != 0)";
  }

  sigma_ = sqrt(log(1 + cv_ * cv_));
  for (auto value : values) score_ += log(value) + 0.5 * pow(log(value / mu_) / sigma_ + sigma_ * 0.5, 2);

  return score_;
}

} /* namespace additionalpriors */
} /* namespace niwa */
