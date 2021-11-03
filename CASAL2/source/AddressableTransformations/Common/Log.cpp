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
namespace addressableransformations {

/**
 * Default constructor
 */
Log::Log(shared_ptr<Model> model) : EstimableTransformation(model) {

  RegisterAsAddressable(PARAM_LOG_PARAMETER, &log_value_);
}


/**
 * Validate
 */
void Log::DoValidate() {
  if(parameter_labels_.size() != 1) {
    LOG_ERROR_P(PARAM_PARAMETER_LABELS) << "Log transformation only can transform 1 parameter at a time. You supplied " << parameter_labels_.size() << " parmaters" ;
  }
}

/**
 * Build
 */
void Log::DoBuild() {
  restored_values_.resize(parameter_labels_.size(), 0.0);
  log_value_ = log(init_values_[0]);
  restored_values_[0] = exp(log_value_);
  // Check the transformations are correct
  for(unsigned i = 0; i < parameter_labels_.size(); ++i) {
    if(restored_values_[i] !=  init_values_[i]) {
      LOG_FINE() << "i = " << i << " restored val " << restored_values_[i]  << " init value = " << init_values_[i];
    }
  }  
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
  if(calculate_jacobian_)
    jacobian_ = -1.0 * log_value_;
  return jacobian_;
}
/**
 * Return the restored value
 */
Double  Log::GetRestoredValue(unsigned index) {
   if(index == 0) {
     return restored_values_[0];
   }
   return restored_values_[1];
}
 /**
 * Report stuff for this transformation
 */
void Log::FillReportCache(ostringstream& cache) {
  LOG_FINE() << "FillReportCache";
  cache << PARAM_PARAMETER_LABELS << ": ";
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
} /* namespace estimabletransformations */
} /* namespace niwa */
