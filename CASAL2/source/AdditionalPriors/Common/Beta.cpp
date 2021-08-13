/**
 * @file Beta.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */

// headers
#include "Beta.h"

#include "../../Estimates/Manager.h"
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
Beta::Beta(shared_ptr<Model> model) : AdditionalPrior(model) {
  parameters_.Bind<Double>(PARAM_MU, &mu_, "Beta distribution mean $\\mu$ parameter", "");
  parameters_.Bind<Double>(PARAM_SIGMA, &sigma_, "Beta distribution variance $\\sigma$ parameter", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_A, &a_, "Beta distribution lower bound, of the range $A$ parameter", "");
  parameters_.Bind<Double>(PARAM_B, &b_, "Beta distribution upper bound of the range $B$ parameter", "");
}

/**
 * Populate any parameters,
 * Validate that values are within expected ranges when bind<>() overloads cannot be used
 *
 * Note: all parameters are populated from configuration files
 */
void Beta::DoValidate() {
  if (a_ >= b_)
    LOG_ERROR_P(PARAM_B) << "value b (" << AS_DOUBLE(b_) << ") cannot be less than or equal to a (" << AS_DOUBLE(a_) << ")";
  Double max_sigma = ((((mu_ - a_) * (b_ - mu_)) / (sigma_ * sigma_)) - 1);
  if (max_sigma <= 0.0)
    LOG_ERROR_P(PARAM_SIGMA) << "value (" << AS_DOUBLE(sigma_) << ") is invalid. (" << mu_ << " - " << a_ << ") * (" << b_ << " - " << mu_ << ") / (" << sigma_ << " * " << sigma_
                             << ") - 1.0 == " << max_sigma << " <= 0.0";
}

/**
 * Build the parameters
 */
void Beta::DoBuild() {
  string error = "";
  if (!model_->objects().VerfiyAddressableForUse(parameter_, addressable::kLookup, error)) {
    LOG_FATAL_P(PARAM_PARAMETER) << "could not be verified for use in additional_prior.vector_average. Error: " << error;
  }

  addressable::Type addressable_type = model_->objects().GetAddressableType(parameter_);
  LOG_FINEST() << "type = " << addressable_type;
  switch (addressable_type) {
    case addressable::kInvalid:
      LOG_CODE_ERROR() << "Invalid addressable type: " << parameter_;
      break;
    case addressable::kSingle:
      addressable_ = model_->objects().GetAddressable(parameter_);
      break;
    default:
      LOG_ERROR() << "The addressable provided for use in additional priors '" << parameter_ << "' has a type that is not supported for Beta additional priors";
      break;
  }
}

/**
 * Get the score
 * @return The score
 */
Double Beta::GetScore() {
  Double value = (*addressable_);
  if (b_ < value || a_ > value) {
    LOG_FATAL_P(PARAM_B) << "parameter b cannot be less than the target parameter (" << value << "), and parameter a cannot be greater than the target parameter";
  }

  Double score_ = 0.0;
  v_            = (mu_ - a_) / (b_ - a_);
  t_            = (((mu_ - a_) * (b_ - mu_)) / (sigma_ * sigma_)) - 1.0;
  m_            = t_ * v_;
  n_            = t_ * (1.0 - v_);
  score_        = ((1.0 - m_) * log(value - a_)) + ((1.0 - n_) * log(b_ - value));
  LOG_FINEST() << "score = " << score_ << " value = " << value << " t = " << t_ << " m_ = " << m_;

  return score_;
}

} /* namespace additionalpriors */
} /* namespace niwa */
