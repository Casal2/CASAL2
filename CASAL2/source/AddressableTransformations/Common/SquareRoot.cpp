/**
 * @file SquareRoot.cpp
 * @author A. Dunn
 * @date Oct, 2022
 * @section LICENSE
 *
 * Copyright NIWA Science �2022 - www.niwa.co.nz
 *
 */

// headers
#include "SquareRoot.h"

#include "../../Estimates/Estimate.h"
#include "../../Estimates/Manager.h"
#include "../../Model/Managers.h"
#include "../../Model/Model.h"
#include "../../Model/Objects.h"

// namespaces
namespace niwa {
namespace addressabletransformations {

/**
 * Default constructor
 */
SquareRoot::SquareRoot(shared_ptr<Model> model) : AddressableTransformation(model) {
  RegisterAsAddressable(PARAM_SQUARE_ROOT_PARAMETER, &SquareRoot_value_);
}

/**
 * Validate
 */
void SquareRoot::DoValidate() {
  if (parameter_labels_.size() != 1) {
    LOG_ERROR_P(PARAM_PARAMETERS) << "The sqrt transformation only can transform 1 parameter at a time. You supplied " << parameter_labels_.size() << " parmaters";
  }
  restored_values_.resize(parameter_labels_.size(), 0.0);
  SquareRoot_value_   = sqrt(init_values_[0]);  // this will get over-ridden by load estimables
  restored_values_[0] = (SquareRoot_value_ * SquareRoot_value_);
  // Check the transformations are correct
  for (unsigned i = 0; i < parameter_labels_.size(); ++i) {
    if (restored_values_[i] != init_values_[i]) {
      LOG_FINE() << "i = " << i << " restored val " << restored_values_[i] << " init value = " << init_values_[i];
    }
  }
}

/**
 * Build
 */
void SquareRoot::DoBuild() {}

/**
 * Restore
 */
void SquareRoot::DoRestore() {
  restored_values_[0] = (SquareRoot_value_ * SquareRoot_value_);
  LOG_FINE() << "Setting Value to: " << restored_values_[0];
  (this->*restore_function_)(restored_values_);
}

/**
 * Get Score
 * @return Jacobian if transforming with Jacobian, otherwise 0.0
 */
Double SquareRoot::GetScore() {
  LOG_TRACE()
  if (prior_applies_to_restored_parameters_)
    jacobian_ = log(1.0 / (2.0 * sqrt(SquareRoot_value_)));
  return jacobian_;
}
/**
 * PrepareForObjectiveFunction
 * if prior_applies_to_restored_parameters_ then set SquareRoot_value_ = (SquareRoot_value_ * SquareRoot_value_)
 */
void SquareRoot::PrepareForObjectiveFunction() {
  if (prior_applies_to_restored_parameters_)
    SquareRoot_value_ = (SquareRoot_value_ * SquareRoot_value_);
}

/**
 * RestoreForObjectiveFunction
 * if prior_applies_to_restored_parameters_ then set SquareRoot_value_ = sqrt(SquareRoot_value_)
 */
void SquareRoot::RestoreForObjectiveFunction() {
  if (prior_applies_to_restored_parameters_)
    SquareRoot_value_ = sqrt(SquareRoot_value_);
}
/**
 * Report stuff for this transformation
 */
void SquareRoot::FillReportCache(ostringstream& cache) {
  LOG_FINE() << "FillReportCache";
  cache << PARAM_PARAMETERS << ": ";
  for (unsigned i = 0; i < parameter_labels_.size(); ++i) cache << parameter_labels_[i] << " ";
  cache << REPORT_EOL;
  cache << "parameter_values: ";
  for (unsigned i = 0; i < restored_values_.size(); ++i) cache << restored_values_[i] << " ";
  cache << REPORT_EOL;
  cache << PARAM_SQUARE_ROOT_PARAMETER << ": " << SquareRoot_value_ << REPORT_EOL;
  cache << "negative_sqrt_jacobian: " << jacobian_ << REPORT_EOL;
}

/**
 * Report stuff for this transformation
 */
void SquareRoot::FillTabularReportCache(ostringstream& cache, bool first_run) {
  LOG_FINEST() << "FillTabularReportCache";
  if (first_run) {
    cache << PARAM_SQUARE_ROOT_PARAMETER << " " << parameter_labels_[0] << " negative_sqrt_jacobian" << REPORT_EOL;
  }
  cache << SquareRoot_value_ << " " << restored_values_[0] << " " << jacobian_ << REPORT_EOL;
}
} /* namespace addressabletransformations */
} /* namespace niwa */
