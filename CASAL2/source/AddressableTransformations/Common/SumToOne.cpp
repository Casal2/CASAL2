/**
 * @file SumToOne.cpp
 * @author C.Marsh
 * @github https://github.com/
 * @date Jan/11/2018
 * @section LICENSE
 *
 * Copyright NIWA Science �2018 - www.niwa.co.nz
 *
 */

// headers
#include "SumToOne.h"


// namespaces
namespace niwa {
namespace addressableransformations {
namespace utils = niwa::utilities;
/**
 * Default constructor
 */
SumToOne::SumToOne(shared_ptr<Model> model) : AddressableTransformation(model) {
  RegisterAsAddressable(PARAM_DIFFERENCE_PARAMETER, &difference_parameter_);
}

/**
 * Validate objects
 */
void SumToOne::DoValidate() {
  restored_values_.resize(parameter_labels_.size(), 0.0);
  if(parameter_labels_.size() != 2) {
    LOG_ERROR_P(PARAM_PARAMETER_LABELS) << "the " << type_ << " transformation only can transform 2 parameter at a time. You supplied " << parameter_labels_.size() << " parmaters" ;
  }
}

/**
 * Build objects
 */
void SumToOne::DoBuild() {
  LOG_TRACE();
  LOG_FINE() << "check values";
  difference_parameter_ = init_values_[0];

  restored_values_[0] = difference_parameter_;
  restored_values_[1] = 1.0 - difference_parameter_;
  // Check the transformations are correct
  for(unsigned i = 0; i < parameter_labels_.size(); ++i) {
    if(restored_values_[i] !=  init_values_[i])
      LOG_CODE_ERROR() << "restored_values_[i] !=  init_values_[i]";
  }

  if(calculate_jacobian_)
    LOG_FATAL_P(PARAM_LABEL) << "You are trying to estimate (look for the @estimate block) a parameter from this class with jacobian true. That isn't derived for this class. Please make sure you want to do this.";
}

/**
 * Restore
 * This method will restore values provided by the minimiser that need to be restored for use in the annual cycle
 */
void SumToOne::DoRestore() {
  restored_values_[0] = difference_parameter_;
  restored_values_[1] = 1.0 - difference_parameter_;
  (this->*restore_function_)(restored_values_);
}

/**
 * GetScore
 * Calculate the Jacobian, to offset the bias of the transformation that enters the objective function
 */
Double SumToOne::GetScore() {
  jacobian_ = 0.0;
  return jacobian_;
}
/**
 * Return the restored value
 */
Double  SumToOne::GetRestoredValue(unsigned index) {
   if(index == 0) {
     return restored_values_[0];
   }
   return restored_values_[1];
}
/**
 * Report stuff for this transformation
 */
void SumToOne::FillReportCache(ostringstream& cache) {
  LOG_FINE() << "FillReportCache";
  cache << PARAM_PARAMETER_LABELS << ": ";
  for(unsigned i = 0; i < parameter_labels_.size(); ++i)
    cache << parameter_labels_[i] << " ";
  cache << REPORT_EOL;
  cache << "parameter_values: ";
  for(unsigned i = 0; i < restored_values_.size(); ++i)
    cache << restored_values_[i] << " ";
  cache << REPORT_EOL;
  cache << PARAM_DIFFERENCE_PARAMETER << ": " << difference_parameter_ << REPORT_EOL;
  cache << "negative_log_jacobian: " << jacobian_ << REPORT_EOL;
}
} /* namespace estimabletransformations */
} /* namespace niwa */
