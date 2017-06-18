/**
 * @file Log.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 8, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 */

// headers
#include "Log.h"

#include "Model/Model.h"
#include "Model/Objects.h"
#include "Model/Managers.h"
#include "Estimates/Manager.h"
#include "Estimates/Estimate.h"

// namespaces
namespace niwa {
namespace estimatetransformations {

/**
 * Default constructor
 */
Log::Log(Model* model) : EstimateTransformation(model) {
  //parameters_.Bind<string>(PARAM_ESTIMATE, &estimate_label_, "The parameter to use in the log transformation", "");
}

/**
 */
void Log::DoValidate() {

}

/**
 *
 */
void Log::DoBuild() {
  LOG_FINEST() << "transformation on @estimate " << estimate_label_;

  estimate_ = model_->managers().estimate()->GetEstimateByLabel(estimate_label_);

  if (estimate_ == nullptr) {
    LOG_ERROR_P(PARAM_ESTIMATE) << "Estimate " << estimate_label_ << " could not be found. Have you defined it?";
    return;
  }

  LOG_FINEST() << "found estimate";

  original_lower_bound_ = estimate_->lower_bound();
  original_upper_bound_ = estimate_->upper_bound();
  original_value_ = estimate_->value();
  //if (!parameters_.Get(PARAM_LOWER_BOUND)->has_been_defined())
    lower_bound_ = log(original_lower_bound_);
  //if (!parameters_.Get(PARAM_UPPER_BOUND)->has_been_defined())
    upper_bound_ = log(original_upper_bound_);
  LOG_FINEST() << "Finish DoBuild()";

}

/**
 *
 */
void Log::Transform() {
  LOG_MEDIUM() << "parameter before transform = " << estimate_->value() << " lower bound " << lower_bound_ << " upper bound " << upper_bound_;
  current_untransformed_value_ = original_value_ = estimate_->value();
  estimate_->set_lower_bound(lower_bound_);
  estimate_->set_upper_bound(upper_bound_);
  estimate_->set_value(log(estimate_->value()));
  LOG_MEDIUM() << "parameter after transform = " << estimate_->value() << " lower bound " << estimate_->lower_bound() << " upper bound " << estimate_->upper_bound();

}

/**
 *
 */
void Log::Restore() {
  estimate_->set_lower_bound(original_lower_bound_);
  estimate_->set_upper_bound(original_upper_bound_);
  estimate_->set_value(exp(estimate_->value()));
}

Double Log::GetScore() {
//
  if(transform_with_jacobian_) {
    jacobian_ = 1.0 / current_untransformed_value_;
    LOG_MEDIUM() << "jacobian: " << jacobian_;
    return jacobian_;
  } else
    return 0.0;
}

/**
 * Get the target addressables so we can ensure each
 * object is not referencing multiple ones as this would
 * cause chain issues
 *
 * @return Set of addressable labels
 */
std::set<string> Log::GetTargetEstimates() {
  set<string> result;
  result.insert(estimate_label_);
  return result;
}
} /* namespace estimatetransformations */
} /* namespace niwa */
