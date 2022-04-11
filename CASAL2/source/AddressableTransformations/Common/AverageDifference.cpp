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
namespace addressabletransformations {

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
  if(parameter_labels_.size() > 2) { // could be one
    LOG_ERROR_P(PARAM_PARAMETERS) << "the average difference transformation only can transform 2 parameter at a time. You supplied " << parameter_labels_.size() << " parmaters" ;
  }
  restored_values_.resize(2, 0.0);
  // if -i use given values average_parameter_ and difference_parameter_ and restore.
  restored_values_[0] = average_parameter_ + (difference_parameter_ / 2.0);
  restored_values_[1] = average_parameter_ - (difference_parameter_ / 2.0);

  // else use config addressables and calculate average_parameter_ and difference_parameter_
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


}

/**
 * Build objects
 */
void AverageDifference::DoBuild() {
  LOG_FINE() << "check values";
  // Check the transformations are correct
  for(unsigned i = 0; i < parameter_labels_.size(); ++i) {
    LOG_FINE() <<" i  = " << i << " restored = " << restored_values_[i]; 
  }  
  
  if(prior_applies_to_restored_parameters_)
    LOG_FATAL_P(PARAM_PRIOR_APPLIES_TO_RESTORED_PARAMETERS) << "There is no jacobian calculated for this transformation. Statistically this may be in in-appropriate, so you are not allowed to do it";

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
 * GetScore
 * @return Jacobian if transformed with Jacobian, otherwise 0.0
 */
Double AverageDifference::GetScore() {
  LOG_TRACE()
  jacobian_ = 0.0;
  return jacobian_;
}

/**
 * PrepareForObjectiveFunction
 * if prior_applies_to_restored_parameters_
 */
void AverageDifference::PrepareForObjectiveFunction() {

}

/**
 * RestoreForObjectiveFunction
 * if prior_applies_to_restored_parameters_
 */
void AverageDifference::RestoreForObjectiveFunction() {

}

/**
 * Report stuff for this transformation
 */
void AverageDifference::FillReportCache(ostringstream& cache) {
  LOG_FINE() << "FillReportCache";
  cache << PARAM_PARAMETERS << ": ";
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

/**
 * Report stuff for this transformation
 */
void AverageDifference::FillTabularReportCache(ostringstream& cache, bool first_run)  {
  LOG_FINEST() << "FillTabularReportCache";
  if(first_run) {
    cache << PARAM_AVERAGE_PARAMETER << " " << PARAM_DIFFERENCE_PARAMETER << " " << parameter_labels_[0] << " " << parameter_labels_[1] << " negative_log_jacobian" << REPORT_EOL;
  }
  cache << average_parameter_ << " " << difference_parameter_ << " " << restored_values_[0] << " " << restored_values_[1] << " " << jacobian_ << REPORT_EOL;
  
}
} /* namespace addressabletransformations */
} /* namespace niwa */
