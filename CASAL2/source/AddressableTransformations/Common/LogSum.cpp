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
namespace addressableransformations {

/**
 * Default constructor
 */
LogSum::LogSum(shared_ptr<Model> model) : EstimableTransformation(model) {
    // Assign memory for transformed addressables
    RegisterAsAddressable(PARAM_LOG_TOTAL_PARAMETER, &log_total_parameter_);
    RegisterAsAddressable(PARAM_TOTAL_DIFFERENCE_PARAMETERS, &total_difference_parameter_);

}

/**
 * Validate objects
 */
void LogSum::DoValidate() {
  total_difference_parameter_ = 0.0;
  log_total_parameter_ = 0.0;
  number_of_parameters_ = parameter_labels_.size();
  restored_values_.resize(number_of_parameters_, 0.0);
  if(parameter_labels_.size() != 2) {
    LOG_ERROR_P(PARAM_PARAMETER_LABELS) << "the " << type_ << " transformation only can transform 2 parameter at a time. You supplied " << parameter_labels_.size() << " parmaters" ;
  }

}
/**
 * Return the restored value
 */
Double  LogSum::GetRestoredValue(unsigned index) {
   if(index == 0) {
     return restored_values_[0];
   }
   return restored_values_[1];
}
/**
 * Build objects
 */
void LogSum::DoBuild() {
  LOG_FINE() << "check values";
  total_parameter_ = 0;
  for(unsigned i = 0; i < parameter_labels_.size(); ++i) {
    LOG_FINE() << parameter_labels_[i] << " value = " << init_values_[i];
    total_parameter_ += init_values_[i];
  }
  log_total_parameter_ = log(total_parameter_);
  total_difference_parameter_ = init_values_[0] / total_parameter_;

  restored_values_[0] = total_parameter_ * total_difference_parameter_;
  restored_values_[1] = total_parameter_ * (1 - total_difference_parameter_);
  // Check the transformations are correct
  for(unsigned i = 0; i < parameter_labels_.size(); ++i) {
    if(restored_values_[i] !=  init_values_[i])
      LOG_CODE_ERROR() << "restored_values_[i] !=  init_values_[i]";
  }
  
  if(calculate_jacobian_)
    LOG_FATAL_P(PARAM_LABEL) << "You are trying to estimate (look for the @estimate block) a parameter from this class with jacobian true. That isn't derived for this class. Please make sure you want to do this.";

}


/**
 * Restore objects
 */
void LogSum::DoRestore() {
  total_parameter_ = exp(log_total_parameter_);
  restored_values_[0] = total_parameter_ * total_difference_parameter_;
  restored_values_[1] = total_parameter_ * (1 - total_difference_parameter_);
  LOG_MEDIUM() << restored_values_[0] << " " << restored_values_[1];

  (this->*restore_function_)(restored_values_);
}

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
  cache << PARAM_PARAMETER_LABELS << ": ";
  for(unsigned i = 0; i < parameter_labels_.size(); ++i)
    cache << parameter_labels_[i] << " ";
  cache << REPORT_EOL;
  cache << "parameter_values: ";
  for(unsigned i = 0; i < restored_values_.size(); ++i)
    cache << restored_values_[i] << " ";
  cache << REPORT_EOL;
  cache << PARAM_LOG_TOTAL_PARAMETER << ": " << log_total_parameter_ << REPORT_EOL;
  cache << PARAM_TOTAL_DIFFERENCE_PARAMETERS << ": " <<  total_difference_parameter_ << REPORT_EOL;
  cache << "negative_log_jacobian: " << jacobian_ << REPORT_EOL;
}
} /* namespace estimabletransformations */
} /* namespace niwa */
