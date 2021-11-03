/**
 * @file Orthogonal.cpp
 * @author C.Marsh
 * @date 13/04/2017
 * @section LICENSE
 *
 * Copyright NIWA Science �2017 - www.niwa.co.nz
 *
 */

// headers
#include "Orthogonal.h"

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
Orthogonal::Orthogonal(shared_ptr<Model> model) : AddressableTransformation(model) {
  RegisterAsAddressable(PARAM_PRODUCT_PARAMETER, &product_parameter_);
  RegisterAsAddressable(PARAM_QUOTIENT_PARAMETER, &quotient_parameter_);

}

/**
 * Validate
 */
void Orthogonal::DoValidate() {
  if(parameter_labels_.size() != 2) {
    LOG_ERROR_P(PARAM_PARAMETER_LABELS) << "the " << type_ << " transformation only can transform 2 parameters at a time. You supplied " << parameter_labels_.size() << " parmaters" ;
  }
  restored_values_.resize(parameter_labels_.size(), 0.0);
}

/**
 * Build
 */
void Orthogonal::DoBuild() {
  LOG_FINE() << "check values";
  for(unsigned i = 0; i < parameter_labels_.size(); ++i) {
    LOG_FINE() << parameter_labels_[i] << " value = " << init_values_[i];
  }
  product_parameter_ =  init_values_[0]  *  init_values_[1] ;
  quotient_parameter_ =  init_values_[0]  /  init_values_[1] ;
  restored_values_[0] = sqrt(product_parameter_ * quotient_parameter_);
  restored_values_[1] = sqrt(product_parameter_/ quotient_parameter_);
  for(unsigned i = 0; i < parameter_labels_.size(); ++i) {
    if(restored_values_[i] !=  init_values_[i])
      LOG_CODE_ERROR() << "restored_values_[i] !=  init_values_[i]";
  }
  
  if(calculate_jacobian_)
    LOG_FATAL_P(PARAM_LABEL) << "You are trying to estimate (look for the @estimate block) a parameter from this class with jacobian true. That isn't derived for this class. Please make sure you want to do this.";

}


/**
 *  Restore estimate
 */
void Orthogonal::DoRestore() {
  restored_values_[0] = sqrt(product_parameter_ * quotient_parameter_);
  restored_values_[1] = sqrt(product_parameter_/ quotient_parameter_);
  LOG_MEDIUM() << restored_values_[0] << " " << restored_values_[1];
  (this->*restore_function_)(restored_values_);

}

/**
 * GetScore
 * @return Jacobian
 */
Double Orthogonal::GetScore() {
  jacobian_ = 0.0;
  return jacobian_;
}
/**
 * Return the restored value
 */
Double  Orthogonal::GetRestoredValue(unsigned index) {
   if(index == 0) {
     return restored_values_[0];
   }
   return restored_values_[1];
}
/**
 * Report stuff for this transformation
 */
void Orthogonal::FillReportCache(ostringstream& cache) {
  LOG_FINE() << "FillReportCache";
  cache << PARAM_PARAMETER_LABELS << ": ";
  for(unsigned i = 0; i < parameter_labels_.size(); ++i)
    cache << parameter_labels_[i] << " ";
  cache << REPORT_EOL;
  cache << "parameter_values: ";
  for(unsigned i = 0; i < restored_values_.size(); ++i)
    cache << restored_values_[i] << " ";
  cache << REPORT_EOL;
  cache << PARAM_PRODUCT_PARAMETER << ": " << product_parameter_ << REPORT_EOL;
  cache << PARAM_QUOTIENT_PARAMETER << ": " <<  quotient_parameter_ << REPORT_EOL;
  cache << "negative_log_jacobian: " << jacobian_ << REPORT_EOL;
}
} /* namespace estimabletransformations */
} /* namespace niwa */
