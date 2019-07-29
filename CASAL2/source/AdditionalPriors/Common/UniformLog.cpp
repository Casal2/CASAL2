/**
 * @file UniformLog.cpp
 * @author C.Marsh
 * @github https://github.com/Zaita
 * @date 3/7/2017
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "UniformLog.h"

#include "Model/Model.h"
#include "Model/Objects.h"

// namespaces
namespace niwa {
namespace additionalpriors {

/**
 * Default constructor
 *
 * Bind any parameters that are allowed to be loaded from the configuration files.
 * Set bounds on registered parameters
 * Register any parameters that can be an estimated or utilised in other run modes (e.g profiling, yields, projections etc)
 * Set some initial values
 *
 * Note: The constructor is parsed to generate Latex for the documentation.
 */
UniformLog::UniformLog(Model* model) : AdditionalPrior(model) {

}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when we cannot use bind<>() overloads
 *
 * Note: all parameters are populated from configuration files
 */
void UniformLog::DoValidate() {

}

void UniformLog::DoBuild() {
	string error = "";
	if (!model_->objects().VerfiyAddressableForUse(parameter_, addressable::kLookup, error)) {
		LOG_FATAL_P(PARAM_PARAMETER) << "could not be verified for use in additional_prior.uniform_log. Error was " << error;
	}

  addressable::Type addressable_type = model_->objects().GetAddressableType(parameter_);
  LOG_FINEST() << "type = " << addressable_type;
  switch(addressable_type) {
    case addressable::kInvalid:
      LOG_CODE_ERROR() << "Invalid addressable type: " << parameter_;
      break;
    case addressable::kSingle:
    	addressable_ = model_->objects().GetAddressable(parameter_);
      break;
    default:
      LOG_ERROR() << "The addressable you have provided for use in a additional priors: " << parameter_ << " is not a type that is supported for Uniform_log additional priors";
      break;
  }

}

/**
 * Return the score for
 */
Double UniformLog::GetScore() {
  Double value = (*addressable_);
  score_ = log(value);
  LOG_FINEST() << "score = " << score_ << " value = " << value ;
  return score_;
}

} /* namespace additionalpriors */
} /* namespace niwa */
