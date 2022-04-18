/**
 * @file Ratio.cpp
 * @author C.Marsh
 * @github https://github.com/Craig44
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 */

// headers
#include "Ratio.h"

#include "../../Estimates/Manager.h"
#include "../../Model/Model.h"
#include "../../Model/Objects.h"

// namespaces
namespace niwa {
namespace additionalpriors {

/**
 * Default constructor
 */
Ratio::Ratio(shared_ptr<Model> model) : AdditionalPrior(model) {
  parameters_.Bind<string>(PARAM_SECOND_PARAMETER, &second_parameter_, "The name of the parameter on the denominator", "");
  parameters_.Bind<Double>(PARAM_MU, &mu_, "The lognormal prior mean (mu) of the ratio", "")->set_lower_bound(0.0, false);
  parameters_.Bind<Double>(PARAM_CV, &cv_, "The lognormal CV parameter for the ratio", "")->set_lower_bound(0.0, false);
}

/**
 * Validate the parameters
 */
void Ratio::DoValidate() {}

/**
 * Build the parameters
 */
void Ratio::DoBuild() {
  LOG_TRACE();
  string error = "";
  if (!model_->objects().VerifyAddressableForUse(second_parameter_, addressable::kLookup, error)) {
    LOG_FATAL_P(PARAM_SECOND_PARAMETER) << "could not be found. Error: " << error;
  }
  error = "";
  if (!model_->objects().VerifyAddressableForUse(parameter_, addressable::kLookup, error)) {
    LOG_FATAL_P(PARAM_PARAMETER) << "could not be found. Error: " << error;
  }

  // first parameter
  addressable::Type addressable_type = model_->objects().GetAddressableType(parameter_);
  LOG_FINEST() << "addressable type = " << addressable_type;
  switch (addressable_type) {
    case addressable::kInvalid:
      LOG_CODE_ERROR() << "Invalid addressable type: " << parameter_;
      break;
    case addressable::kSingle:
      addressable_ = model_->objects().GetAddressable(parameter_);
      break;
    default:
      LOG_ERROR() << "The addressable provided '" << parameter_ << "' has a type that is not supported for element difference additional priors";
      break;
  }

  // Get second parameter estimates
  addressable_type = model_->objects().GetAddressableType(second_parameter_);
  LOG_FINEST() << "addressable type = " << addressable_type;
  switch (addressable_type) {
    case addressable::kInvalid:
      LOG_CODE_ERROR() << "Invalid addressable type: " << second_parameter_;
      break;
    case addressable::kSingle:
      second_addressable_ = model_->objects().GetAddressable(second_parameter_);
      break;
    default:
      LOG_ERROR() << "The addressable provided '" << second_parameter_ << "' has a type that is not supported for element difference additional priors";
      break;
  }
  sigma_ = sqrt(log(1.0+cv_*cv_));
}

/**
 * Get the score for this penalty
 * @return The penalty score
 */
Double Ratio::GetScore() {
  LOG_TRACE();
  Double ratio = (*addressable_) / (*second_addressable_);
  Double score = log((*addressable_)) + 0.5 * pow(log(ratio / mu_)/sigma_ + sigma_*0.5, 2);
  LOG_FINEST() << (*addressable_) << "/" << (*second_addressable_) << " = " << ratio <<" score = " << score << " mu " << mu_ << " sigma = " << sigma_;
  return score;
}

} /* namespace additionalpriors */
} /* namespace niwa */
