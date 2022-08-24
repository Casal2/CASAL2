/**
 * @file Logistic.cpp
 * @author C.Marsh
 * @github https://github.com/Craig44
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 */

// headers
#include "Logistic.h"

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
Logistic::Logistic(shared_ptr<Model> model) : AddressableTransformation(model) {
  parameters_.Bind<Double>(PARAM_LOWER_BOUND, &lower_bound_, "Lower bound of parameter space", "", true);
  parameters_.Bind<Double>(PARAM_UPPER_BOUND, &upper_bound_, "Upper bound of parameter space", "", true);
  RegisterAsAddressable(PARAM_LOGISTIC_PARAMETER, &logistic_value_);
}


/**
 * Validate
 */
void Logistic::DoValidate() {
  if(parameter_labels_.size() != 1) {
    LOG_ERROR_P(PARAM_PARAMETERS) << "Logistic transformation only can transform 1 parameter at a time. You supplied " << parameter_labels_.size() << " parmaters" ;
  }
  restored_values_.resize(parameter_labels_.size(), 0.0);
  if(init_values_[0] == lower_bound_)
    LOG_ERROR_P(PARAM_PARAMETERS) << "initial value was equal to lower bound. This will cause an Inf and is not allowed. Change starting value";
    if(init_values_[0] == upper_bound_)
    LOG_ERROR_P(PARAM_PARAMETERS) << "initial value was equal to upper bound. This will cause an Inf and is not allowed. Change starting value";
  
  logistic_value_ = utilities::math::logit_bounds(init_values_[0], lower_bound_, upper_bound_); // this will get over-riden by load estimables
  restored_values_[0] = utilities::math::invlogit_bounds(logistic_value_, lower_bound_, upper_bound_);
  // Check the transformations are correct
  for(unsigned i = 0; i < parameter_labels_.size(); ++i) {
    if(restored_values_[i] !=  init_values_[i]) {
      LOG_FINE() << "i = " << i << " restored val " << restored_values_[i]  << " init value = " << init_values_[i];
    }
  } 
}

/**
 * Build
 */
void Logistic::DoBuild() {

}


/**
 * Restore
 */
void Logistic::DoRestore() {
  restored_values_[0] = utilities::math::invlogit_bounds(logistic_value_, lower_bound_, upper_bound_);
  LOG_FINE() << "Setting Value to: " << restored_values_[0];
  (this->*restore_function_)(restored_values_);
}

/**
 * Get Score
 * @return Jacobian if transforming with Jacobian, otherwise 0.0
 */
Double Logistic::GetScore() {
  LOG_TRACE()
  // TODO add this in.
  if(prior_applies_to_restored_parameters_)
    jacobian_ = (upper_bound_ - lower_bound_) * utilities::math::invlogit(logistic_value_) * (1 - utilities::math::invlogit(logistic_value_));
  
  return jacobian_;
}
/**
 * PrepareForObjectiveFunction
 * if prior_applies_to_restored_parameters_ then set log_value_ = exp(log_value_)
 */
void Logistic::PrepareForObjectiveFunction() {
  if(prior_applies_to_restored_parameters_)
    logistic_value_ = utilities::math::invlogit_bounds(logistic_value_, lower_bound_, upper_bound_); // this will get over-riden by load estimables
}

/**
 * RestoreForObjectiveFunction
 * if prior_applies_to_restored_parameters_ then set log_value_ = log(log_value_)
 */
void Logistic::RestoreForObjectiveFunction() {
  if(prior_applies_to_restored_parameters_)
    logistic_value_ = utilities::math::logit_bounds(logistic_value_, lower_bound_, upper_bound_); // this will get over-riden by load estimables
}

 /**
 * Report stuff for this transformation
 */
void Logistic::FillReportCache(ostringstream& cache) {
  LOG_FINE() << "FillReportCache";
  cache << PARAM_PARAMETERS << ": ";
  for(unsigned i = 0; i < parameter_labels_.size(); ++i)
    cache << parameter_labels_[i] << " ";
  cache << REPORT_EOL;
  cache << "parameter_values: ";
  for(unsigned i = 0; i < restored_values_.size(); ++i)
    cache << restored_values_[i] << " ";
  cache << REPORT_EOL;
  cache << PARAM_LOWER_BOUND << ": " << lower_bound_ << REPORT_EOL;
  cache << PARAM_UPPER_BOUND << ": " << upper_bound_ << REPORT_EOL;
  cache << PARAM_LOGISTIC_PARAMETER << ": " << logistic_value_ << REPORT_EOL;
  cache << "negative_log_jacobian: " << jacobian_ << REPORT_EOL;
}

/**
 * Report stuff for this transformation
 */
void Logistic::FillTabularReportCache(ostringstream& cache, bool first_run)  {
  LOG_FINEST() << "FillTabularReportCache";
  if(first_run) {
    cache << PARAM_LOGISTIC_PARAMETER << " " << parameter_labels_[0] << " negative_log_jacobian" << REPORT_EOL;
  }  
  cache << logistic_value_ << " " << restored_values_[0] << " " << jacobian_ << REPORT_EOL;
}

} /* namespace addressabletransformations */
} /* namespace niwa */
