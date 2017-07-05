/**
 * @file VectorSmoothing.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 28/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "VectorSmoothing.h"

#include "Estimates/Manager.h"
#include "Model/Model.h"
#include "Model/Objects.h"

// namespaces
namespace niwa {
namespace additionalpriors {

/**
 * Default constructor
 */
VectorSmoothing::VectorSmoothing(Model* model) : AdditionalPrior(model) {
  parameters_.Bind<bool>(PARAM_LOG_SCALE, &log_scale_, "Should sums of squares be calculated on the log scale?", "", false);
  parameters_.Bind<Double>(PARAM_MULTIPLIER, &multiplier_, "Multiply the penalty by this factor", "", 1);
  parameters_.Bind<unsigned>(PARAM_LOWER_BOUND, &lower_, "First element to apply the penalty to in the vector", "", 0u);
  parameters_.Bind<unsigned>(PARAM_UPPER_BOUND, &upper_, "Last element to apply the penalty to in the vector", "", 0u);
  parameters_.Bind<unsigned>(PARAM_R, &r_, "Penalty applied to rth differences", "", 2u);
}

/**
 * Validate our parameters
 */
void VectorSmoothing::DoValidate() {
}

/**
 * Build our parameters
 */
void VectorSmoothing::DoBuild() {
  string error = "";
  if (!model_->objects().VerfiyAddressableForUse(parameter_, addressable::kLookup, error)) {
    LOG_FATAL_P(PARAM_PARAMETER) << "could not be verified for use in additional_prior.vector_smoothing. Error was " << error;
  }

  addressable::Type addressable_type = model_->objects().GetAddressableType(parameter_);
  LOG_FINEST() << "addressable type = " << addressable_type;
  switch(addressable_type) {
    case addressable::kInvalid:
      LOG_CODE_ERROR() << "Invalid addressable type: " << parameter_;
      break;
    case addressable::kVector:
      addressable_vector_ = model_->objects().GetAddressableVector(parameter_);
      break;
    case addressable::kUnsignedMap:
      addressable_map_ = model_->objects().GetAddressableUMap(parameter_);
      break;
    default:
      LOG_ERROR() << "The addressable you have provided for use in a additional priors: " << parameter_
        << " is not a type that is supported for vector smoothing additional priors";
      break;
  }
}

/**
 * Get the score for this penalty
 *
 * @return Penalty score
 */
Double VectorSmoothing::GetScore() {
  vector<Double> values;
  if (addressable_vector_ != nullptr)
    values.assign((*addressable_vector_).begin(), (*addressable_vector_).end());
  else if (addressable_map_ != nullptr) {
    for (auto iter : (*addressable_map_))
      values.push_back(iter.second);
  } else
    LOG_CODE_ERROR() << "(addressable_map_ != 0) && (addressable_vector_ != 0)";

  if(upper_ == lower_ && upper_ == 0u) {
    upper_ = values.size();
    lower_ = 1;
  }

  if(upper_ == lower_)
    LOG_FATAL_P(PARAM_UPPER_BOUND) << "Lower and upper bound cannot be equal";
  if (upper_ > values.size())
    LOG_FATAL_P(PARAM_UPPER_BOUND) << "The last element must not be greater than size of vector";
  if (lower_ < 1)
    LOG_FATAL_P(PARAM_LOWER_BOUND) << "The first element must not be less than 1";

  if (r_ >= (upper_ - lower_))
  LOG_FATAL_P(PARAM_R) << PARAM_R << " R cannot be greater than or equal to size of vector - 1";

  Double score = 0.0;
  if (log_scale_) {
    for (Double& value : values)
      value = log(value);
  }
  for (unsigned i = 1; i <= r_; ++i) {
    for(unsigned j = (lower_ - 1); j <= ((upper_ - 1) - i); ++j) {
      values[j] = values[j + 1] - values[j];
    }
    values[(upper_ - 1) - i + 1] = 0;
  }

  for (unsigned k = (lower_ - 1); k <= (upper_ - 1); ++k)
    score += values[k] * values[k];
  return score * multiplier_;
}

} /* namespace additionalpriors */
} /* namespace niwa */
