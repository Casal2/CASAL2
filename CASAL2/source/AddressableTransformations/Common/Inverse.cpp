/**
 * @file Inverse.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 8, 2016
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "Inverse.h"

#include "../../Model/Model.h"
#include "../../Model/Objects.h"

// namespaces
namespace niwa {
namespace addressabletransformations {

/**
 * Default constructor
 */
Inverse::Inverse(shared_ptr<Model> model) : AddressableTransformation(model) {
  RegisterAsAddressable(PARAM_INVERSE_PARAMETER, &inverse_value_);
}

/**
 * Validate
 */
void Inverse::DoValidate() {
  if (parameter_labels_.size() != 1) {
    LOG_ERROR_P(PARAM_PARAMETERS) << "The inverse transformation can only transform 1 parameter at a time. You supplied " << parameter_labels_.size() << " parmaters";
  }
  restored_values_.resize(parameter_labels_.size(), 0.0);
  // if usage -i calculate restored value
  restored_values_[0] = 1.0 / inverse_value_;
  // else get transform value
  inverse_value_ = 1.0 / init_values_[0];
}

/**
 * Build
 */
void Inverse::DoBuild() {}

/**
 * Restore
 */
void Inverse::DoRestore() {
  restored_values_[0] = 1.0 / inverse_value_;
  LOG_MEDIUM() << "Setting Value to: " << restored_values_[0];
  (this->*restore_function_)(restored_values_);
}

/**
 * GetScore
 * @return log(Jacobian) if transformed with Jacobian, otherwise 0.0
 */
Double Inverse::GetScore() {
  LOG_TRACE()
  if (prior_applies_to_restored_parameters_)
    // -ln(J) = -ln(1/(x^2));
    jacobian_ = 2.0 * log(inverse_value_);
  return jacobian_;
}

/**
 * PrepareForObjectiveFunction
 * if prior_applies_to_restored_parameters_ then set inverse_value_ = 1/inverse_value_
 */
void Inverse::PrepareForObjectiveFunction() {
  if (prior_applies_to_restored_parameters_)
    inverse_value_ = 1.0 / inverse_value_;
}

/**
 * RestoreForObjectiveFunction
 * if prior_applies_to_restored_parameters_ then set inverse_value_ = 1/inverse_value_
 */
void Inverse::RestoreForObjectiveFunction() {
  if (prior_applies_to_restored_parameters_)
    inverse_value_ = 1.0 / inverse_value_;
}
/**
 * Report stuff for this transformation
 */
void Inverse::FillReportCache(ostringstream& cache) {
  LOG_FINE() << "FillReportCache";
  cache << PARAM_PARAMETERS << ": ";
  for (unsigned i = 0; i < parameter_labels_.size(); ++i) cache << parameter_labels_[i] << " ";
  cache << REPORT_EOL;
  cache << "parameter_values: ";
  for (unsigned i = 0; i < restored_values_.size(); ++i) cache << restored_values_[i] << " ";
  cache << REPORT_EOL;
  cache << PARAM_INVERSE_PARAMETER << ": " << inverse_value_ << REPORT_EOL;
  cache << "negative_log_jacobian: " << jacobian_ << REPORT_EOL;
}
/**
 * Report stuff for this transformation
 */
void Inverse::FillTabularReportCache(ostringstream& cache, bool first_run) {
  LOG_FINEST() << "FillTabularReportCache";
  if (first_run) {
    cache << PARAM_INVERSE_PARAMETER << " " << parameter_labels_[0] << " negative_log_jacobian" << REPORT_EOL;
  }
  cache << inverse_value_ << " " << restored_values_[0] << " " << jacobian_ << REPORT_EOL;
}
} /* namespace addressabletransformations */
} /* namespace niwa */
