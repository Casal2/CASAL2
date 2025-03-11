/**
 * @file Orthogonal.cpp
 * @author C.Marsh
 * @date 13/04/2017
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "Orthogonal.h"

#include "../../Estimates/Estimate.h"
#include "../../Estimates/Manager.h"
#include "../../Model/Managers.h"
#include "../../Model/Model.h"
#include "../../Model/Objects.h"
#include "../../Utilities/Math.h"

// namespaces
namespace niwa {
namespace addressabletransformations {

/**
 * Default constructor
 */
Orthogonal::Orthogonal(shared_ptr<Model> model) : AddressableTransformation(model) {
  RegisterAsAddressable(PARAM_PRODUCT_PARAMETER, &product_parameter_);
  RegisterAsAddressable(PARAM_QUOTIENT_PARAMETER, &quotient_parameter_);
}

/**
 * Validate
 */
void Orthogonal::DoValidate() {
  if (parameter_labels_.size() != 2) {  // could be one
    LOG_ERROR_P(PARAM_PARAMETERS) << "the " << type_ << " transformation can only transform 2 parameters at a time. You supplied " << parameter_labels_.size() << " parmaters";
  }
  if (init_values_[0] <= 0) {
    LOG_ERROR_P(PARAM_PARAMETERS) << "the " << type_ << " first value must be positive. Provided value was: " << init_values_[0];
  }

  restored_values_.resize(parameter_labels_.size(), 0.0);
  orthogonalTransform(init_values_[0], init_values_[1], quotient_parameter_, product_parameter_);
  inverseOrthogonalTransform(quotient_parameter_, product_parameter_, restored_values_[0], restored_values_[1]);

  for (unsigned i = 0; i < parameter_labels_.size(); ++i) {
    if (fabs(restored_values_[i] - init_values_[i]) > 1e-5)
      LOG_CODE_ERROR() << "restored_values_[i] !=  init_values_[i]. restored value = " << restored_values_[i] << " init value = " << init_values_[i];
  }

  if (prior_applies_to_restored_parameters_)
    LOG_FATAL_P(PARAM_PRIOR_APPLIES_TO_RESTORED_PARAMETERS)
        << "There is no Jacobian calculated for this transformation. Statistically, this may not be appropriate, so you are not allowed to do it";
}

/**
 * Build
 */
void Orthogonal::DoBuild() {}

/**
 *  Restore estimate
 */
void Orthogonal::DoRestore() {
  inverseOrthogonalTransform(quotient_parameter_, product_parameter_, restored_values_[0], restored_values_[1]);

  LOG_FINE() << restored_values_[0] << " " << restored_values_[1];
  (this->*restore_function_)(restored_values_);
}

/**
 * GetScore
 * @return log(Jacobian) if transformed with Jacobian, otherwise 0.0
 */
Double Orthogonal::GetScore() {
  jacobian_ = 0.0;
  return jacobian_;
}

/**
 * PrepareForObjectiveFunction
 * if prior_applies_to_restored_parameters_
 */
void Orthogonal::PrepareForObjectiveFunction() {}

/**
 * RestoreForObjectiveFunction
 * if prior_applies_to_restored_parameters_
 */
void Orthogonal::RestoreForObjectiveFunction() {}
/**
 * Report stuff for this transformation
 */
void Orthogonal::FillReportCache(ostringstream& cache) {
  LOG_FINE() << "FillReportCache";
  cache << PARAM_PARAMETERS << ": ";
  for (unsigned i = 0; i < parameter_labels_.size(); ++i) cache << parameter_labels_[i] << " ";
  cache << REPORT_EOL;
  cache << "parameter_values: ";
  for (unsigned i = 0; i < restored_values_.size(); ++i) cache << restored_values_[i] << " ";
  cache << REPORT_EOL;
  cache << PARAM_PRODUCT_PARAMETER << ": " << product_parameter_ << REPORT_EOL;
  cache << PARAM_QUOTIENT_PARAMETER << ": " << quotient_parameter_ << REPORT_EOL;
  cache << "negative_log_jacobian: " << jacobian_ << REPORT_EOL;
}

/**
 * Report stuff for this transformation
 */
void Orthogonal::FillTabularReportCache(ostringstream& cache, bool first_run) {
  LOG_FINEST() << "FillTabularReportCache";
  if (first_run) {
    cache << PARAM_PRODUCT_PARAMETER << " " << PARAM_QUOTIENT_PARAMETER << " " << parameter_labels_[0] << " " << parameter_labels_[1] << " negative_log_jacobian" << REPORT_EOL;
  }
  cache << product_parameter_ << " " << quotient_parameter_ << " " << restored_values_[0] << " " << restored_values_[1] << " " << jacobian_ << REPORT_EOL;
}
} /* namespace addressabletransformations */
} /* namespace niwa */
