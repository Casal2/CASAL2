/**
 * @file AverageDifference.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 8, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science �2016 - www.niwa.co.nz
 *
 **/

// headers
#include "AverageDifference.h"

#include "../../Model/Managers.h"
#include "../../Model/Model.h"
#include "../../Model/Objects.h"

// namespaces
namespace niwa {
namespace addressableransformations {

/**
 * Default constructor
 */
AverageDifference::AverageDifference(shared_ptr<Model> model) : AddressableTransformation(model) {
    RegisterAsAddressable(PARAM_AVERAGE_PARAMETER, &average_parameter_);
    RegisterAsAddressable(PARAM_DIFFERENCE_PARAMETER, &difference_parameter_);
}

/**
 * Validate objects
 */
void AverageDifference::DoValidate() {
  if(parameter_labels_.size() != 2) {
    LOG_ERROR_P(PARAM_PARAMETER_LABELS) << "the average difference transformation only can transform 2 parameter at a time. You supplied " << parameter_labels_.size() << " parmaters" ;
  }
  number_of_parameters_ = parameter_labels_.size();
  restored_values_.resize(number_of_parameters_, 0.0);
  average_parameter_ = 0.0;
}

/**
 * Build objects
 */
void AverageDifference::DoBuild() {
  LOG_FINE() << "check values";
  Double total = 0;
  for(unsigned i = 0; i < parameter_labels_.size(); ++i) {
    LOG_FINE() << parameter_labels_[i] << " value = " << init_values_[i];
    total += init_values_[i];
  }
  average_parameter_ = total / 2.0;
  difference_parameter_ = (average_parameter_ - init_values_[1]) * 2.0;
  LOG_FINE() <<"average param = " << average_parameter_;
  LOG_FINE() <<"difference param = " << difference_parameter_;


  restored_values_[0] = average_parameter_ + (difference_parameter_ / 2.0);
  restored_values_[1] = average_parameter_ - (difference_parameter_ / 2.0);
  // Check the transformations are correct
  /*
  for(unsigned i = 0; i < parameter_labels_.size(); ++i) {
    if(restored_values_[i] !=  init_values_[i]) {
      LOG_FINE() <<" i  = " << i << " restored = " << restored_values_[i] << " init val = " << init_values_[i];
      LOG_CODE_ERROR() << "restored_values_[i] !=  init_values_[i]";
    }
  }  
  */
  
  if(calculate_jacobian_)
    LOG_FATAL_P(PARAM_LABEL) << "You are trying to estimate (look for the @estimate block) a parameter from this class with jacobian true. That isn't derived for this class. Please make sure you want to do this.";

}
/**
 * Restore objects
 */
void AverageDifference::DoRestore() {
  restored_values_[0] = average_parameter_ + (difference_parameter_ / 2.0);
  restored_values_[1] = average_parameter_ - (difference_parameter_ / 2.0);
  LOG_MEDIUM() << restored_values_[0] << " " << restored_values_[1];
  (this->*restore_function_)(restored_values_);
}
/**
 * Return the restored value
 */
Double  AverageDifference::GetRestoredValue(unsigned index) {
   if(index == 0) {
     return restored_values_[0];
   }
   return restored_values_[1];
}
/**
 * GetScore
 * @return Jacobian if transformed with Jacobian, otherwise 0.0
 */
Double AverageDifference::GetScore() {
  LOG_TRACE()
  jacobian_ = 0.0;
  return jacobian_;
}
/**
 * Report stuff for this transformation
 */
void AverageDifference::FillReportCache(ostringstream& cache) {
  LOG_FINE() << "FillReportCache";
  cache << PARAM_PARAMETER_LABELS << ": ";
  for(unsigned i = 0; i < parameter_labels_.size(); ++i)
    cache << parameter_labels_[i] << " ";
  cache << REPORT_EOL;
  cache << "parameter_values: ";
  for(unsigned i = 0; i < restored_values_.size(); ++i)
    cache << restored_values_[i] << " ";
  cache << REPORT_EOL;
  cache << PARAM_AVERAGE_PARAMETER << ": " << average_parameter_ << REPORT_EOL;
  cache << PARAM_DIFFERENCE_PARAMETER << ": " <<  difference_parameter_ << REPORT_EOL;
  cache << "negative_log_jacobian: " << jacobian_ << REPORT_EOL;
}
} /* namespace estimabletransformations */
} /* namespace niwa */
