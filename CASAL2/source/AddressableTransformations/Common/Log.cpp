/**
 * @file Log.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 8, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science �2016 - www.niwa.co.nz
 *
 */

// headers
#include "Log.h"

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
Log::Log(shared_ptr<Model> model) : AddressableTransformation(model) {

}


/**
 * Validate
 */
void Log::DoValidate() {
  if(parameter_labels_.size() != 1) {
    LOG_ERROR_P(PARAM_PARAMETERS) << "Log transformation only can transform 1 parameter at a time. You supplied " << parameter_labels_.size() << " parmaters" ;
  }
  restored_values_.resize(parameter_labels_.size(), 0.0);
  log_value_ = log(init_values_[0]); // this will get over-riden by load estimables
  restored_values_[0] = exp(log_value_);
  // Check the transformations are correct
  for(unsigned i = 0; i < parameter_labels_.size(); ++i) {
    if(restored_values_[i] !=  init_values_[i]) {
      LOG_FINE() << "i = " << i << " restored val " << restored_values_[i]  << " init value = " << init_values_[i];
    }
  } 
  RegisterAsAddressable(PARAM_LOG_PARAMETER, &log_value_);
}

/**
 * Build
 */
void Log::DoBuild() {

}


/**
 * Restore
 */
void Log::DoRestore() {
  restored_values_[0] = exp(log_value_);
  LOG_MEDIUM() << "Setting Value to: " << restored_values_[0];
  (this->*restore_function_)(restored_values_);
}

/**
 * Get Score
 * @return Jacobian if transforming with Jacobian, otherwise 0.0
 */
Double Log::GetScore() {
  LOG_TRACE()
  if(prior_applies_to_restored_parameters_)
    jacobian_ = -1.0 * log_value_;
  return jacobian_;
}
/**
 * PrepareForObjectiveFunction
 * if prior_applies_to_restored_parameters_ then set log_value_ = exp(log_value_)
 */
void Log::PrepareForObjectiveFunction() {
  if(prior_applies_to_restored_parameters_)
    log_value_ = exp(log_value_);
}

/**
 * RestoreForObjectiveFunction
 * if prior_applies_to_restored_parameters_ then set log_value_ = log(log_value_)
 */
void Log::RestoreForObjectiveFunction() {
  if(prior_applies_to_restored_parameters_)
    log_value_ = log(log_value_);
}
 /**
 * Report stuff for this transformation
 */
void Log::FillReportCache(ostringstream& cache) {
  LOG_FINE() << "FillReportCache";
  cache << PARAM_PARAMETERS << ": ";
  for(unsigned i = 0; i < parameter_labels_.size(); ++i)
    cache << parameter_labels_[i] << " ";
  cache << REPORT_EOL;
  cache << "parameter_values: ";
  for(unsigned i = 0; i < restored_values_.size(); ++i)
    cache << restored_values_[i] << " ";
  cache << REPORT_EOL;
  cache << PARAM_LOG_PARAMETER << ": " << log_value_ << REPORT_EOL;
  cache << "negative_log_jacobian: " << jacobian_ << REPORT_EOL;
}
} /* namespace addressabletransformations */
} /* namespace niwa */
