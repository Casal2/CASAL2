/**
 * @file SquareRoot.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Dec 7, 2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "SquareRoot.h"

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
SquareRoot::SquareRoot(Model* model) : EstimateTransformation(model) {
  //parameters_.Bind<string>(PARAM_ESTIMATE, &estimate_label_, "The parameter to use in the square root transformation", "");
}

/**
 */
void SquareRoot::DoValidate() {

}

/**
 *
 */
void SquareRoot::DoBuild() {
  LOG_TRACE();
  estimate_ = model_->managers().estimate()->GetEstimateByLabel(estimate_label_);
  if (estimate_ == nullptr) {
    LOG_ERROR_P(PARAM_ESTIMATE) << "Estimate " << estimate_label_ << " could not be found. Have you defined it?";
    return;
  }

  original_lower_bound_ = estimate_->lower_bound();
  original_upper_bound_ = estimate_->upper_bound();

  lower_bound_ = sqrt(original_lower_bound_);
  upper_bound_ = sqrt(original_upper_bound_);
}

/**
 *
 */
void SquareRoot::Transform() {
  LOG_TRACE();
  estimate_->set_lower_bound(lower_bound_);
  estimate_->set_upper_bound(upper_bound_);
  current_untransformed_value_ = estimate_->value();
  estimate_->set_value(sqrt(current_untransformed_value_));
  LOG_MEDIUM() << "transforming value from " << current_untransformed_value_ << " to " << estimate_->value();
}

/**
 *
 */
void SquareRoot::Restore() {
  LOG_TRACE();
  estimate_->set_lower_bound(original_lower_bound_);
  estimate_->set_upper_bound(original_upper_bound_);
  Double check = estimate_->value() * estimate_->value();
  LOG_MEDIUM() << "Restoring value from " << estimate_->value()  << " to " << AS_DOUBLE(check);
  estimate_->set_value(estimate_->value() * estimate_->value());

}

Double SquareRoot::GetScore() {
//
  if(transform_with_jacobian_) {
    jacobian_ = -0.5 * pow(current_untransformed_value_,-1.5);
    LOG_MEDIUM() << "jacobian: " << jacobian_;
    return jacobian_;
  } else
    return 0.0;
}

/**
 * Get the target estimables so we can ensure each
 * object is not referencing multiple ones as this would
 * cause chain issues
 *
 * @return Set of estimable labels
 */
std::set<string> SquareRoot::GetTargetEstimates() {
  set<string> result;
  result.insert(estimate_label_);
  return result;
}

} /* namespace estimatetransformations */
} /* namespace niwa */
