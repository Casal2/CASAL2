/**
 * @file LogSum.cpp
 * @author C.Marsh
 * @github https://github.com/Zaita
 * @date Jul 8, 2017
 * @section LICENSE
 *
 * Copyright NIWA Science �2017 - www.niwa.co.nz
 *
 */

// headers
#include "LogSum.h"

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
LogSum::LogSum(shared_ptr<Model> model) : AddressableTransformation(model) {
  RegisterAsAddressable(PARAM_LOG_TOTAL_PARAMETER, &log_total_parameter_);
  RegisterAsAddressable(PARAM_TOTAL_PROPORTION_PARAMETER, &total_proportion_parameter_);
}

/**
 * Validate objects
 */
void LogSum::DoValidate() {
  total_proportion_parameter_ = 0.0;
  log_total_parameter_        = 0.0;
  number_of_parameters_       = parameter_labels_.size();
  restored_values_.resize(number_of_parameters_, 0.0);
  if (parameter_labels_.size() > 2) {  // could be one
    LOG_ERROR_P(PARAM_PARAMETERS) << "the " << type_ << " transformation only can transform 2 parameters at a time. You supplied " << parameter_labels_.size() << " parmaters";
  }

  LOG_FINE() << "check values";
  total_parameter_ = 0;
  for (unsigned i = 0; i < parameter_labels_.size(); ++i) {
    LOG_FINE() << parameter_labels_[i] << " value = " << init_values_[i];
    total_parameter_ += init_values_[i];
  }
  log_total_parameter_        = log(total_parameter_);
  total_proportion_parameter_ = init_values_[0] / total_parameter_;

  restored_values_[0] = total_parameter_ * total_proportion_parameter_;
  restored_values_[1] = total_parameter_ * (1 - total_proportion_parameter_);
  // Check the transformations are correct
  for (unsigned i = 0; i < parameter_labels_.size(); ++i) {
    if (restored_values_[i] != init_values_[i])
      LOG_CODE_ERROR() << "restored_values_[i] !=  init_values_[i]";
  }
  if (prior_applies_to_restored_parameters_)
    LOG_FATAL_P(PARAM_PRIOR_APPLIES_TO_RESTORED_PARAMETERS)
        << "There is no jacobian for this transformation. Statistically, this may be in inappropriate, so you are not allowed to do it";
}

/**
 * Build objects
 */
void LogSum::DoBuild() {}

/**
 * Restore objects
 */
void LogSum::DoRestore() {
  LOG_MEDIUM() << log_total_parameter_ << " " << total_proportion_parameter_;

  total_parameter_    = exp(log_total_parameter_);
  restored_values_[0] = total_parameter_ * total_proportion_parameter_;
  restored_values_[1] = total_parameter_ * (1 - total_proportion_parameter_);
  LOG_MEDIUM() << restored_values_[0] << " " << restored_values_[1];

  (this->*restore_function_)(restored_values_);
}

/**
 * PrepareForObjectiveFunction
 * if prior_applies_to_restored_parameters_
 */
void LogSum::PrepareForObjectiveFunction() {}

/**
 * RestoreForObjectiveFunction
 * if prior_applies_to_restored_parameters_
 */
void LogSum::RestoreForObjectiveFunction() {}
/**
 * GetScore
 * @return Jacobian if transformed with Jacobian, otherwise 0.0
 */
Double LogSum::GetScore() {
  LOG_TRACE()
  jacobian_ = 0.0;
  return jacobian_;
}

/**
 * Report stuff for this transformation
 */
void LogSum::FillReportCache(ostringstream& cache) {
  LOG_FINE() << "FillReportCache";
  cache << PARAM_PARAMETERS << ": ";
  for (unsigned i = 0; i < parameter_labels_.size(); ++i) cache << parameter_labels_[i] << " ";
  cache << REPORT_EOL;
  cache << "parameter_values: ";
  for (unsigned i = 0; i < restored_values_.size(); ++i) cache << restored_values_[i] << " ";
  cache << REPORT_EOL;
  cache << PARAM_LOG_TOTAL_PARAMETER << ": " << log_total_parameter_ << REPORT_EOL;
  cache << PARAM_TOTAL_PROPORTION_PARAMETER << ": " << total_proportion_parameter_ << REPORT_EOL;
  cache << "negative_log_jacobian: " << jacobian_ << REPORT_EOL;
}

/**
 * Report stuff for this transformation
 */
void LogSum::FillTabularReportCache(ostringstream& cache, bool first_run)  {
  LOG_FINEST() << "FillTabularReportCache";
  if(first_run) {
    cache << PARAM_LOG_TOTAL_PARAMETER << " " << PARAM_TOTAL_PROPORTION_PARAMETER << " " << parameter_labels_[0] << " " << parameter_labels_[1] << " negative_log_jacobian" << REPORT_EOL;
  }
  cache << log_total_parameter_ << " " << total_proportion_parameter_ << " " << restored_values_[0] << " " << restored_values_[1] << " " << jacobian_ << REPORT_EOL;
}
} /* namespace addressabletransformations */
} /* namespace niwa */
