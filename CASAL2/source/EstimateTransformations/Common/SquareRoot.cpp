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
  parameters_.Bind<string>(PARAM_ESTIMATE_LABEL, &estimate_label_, "Label of estimate block to apply transformation. Defined as $\theta_1$ in the documentation", "");
}

/**
 *
 */
void SquareRoot::DoBuild() {
  LOG_TRACE();
  estimate_ = model_->managers().estimate()->GetEstimateByLabel(estimate_label_);
  if (estimate_ == nullptr) {
    LOG_ERROR_P(PARAM_ESTIMATE) << "Estimate " << estimate_label_ << " was not found.";
    return;
  }
  // Initialise for -r runs
  current_untransformed_value_ = estimate_->value();

  LOG_FINE() << "transform with objective = " << transform_with_jacobian_ << " estimate transform "
    << estimate_->transform_for_objective() << " together = " << !transform_with_jacobian_ && !estimate_->transform_for_objective();
  if (!transform_with_jacobian_ && !estimate_->transform_for_objective()) {
    LOG_ERROR_P(PARAM_TRANSFORM_WITH_JACOBIAN) << "A transformation that does not contribute to the Jacobian was specified,"
     << " and the prior parameters do not refer to the transformed estimate, in the @estimate " << estimate_label_
     << ". This is not advised, and may cause bias errors. Please check the User Manual for more info";
  }
  if (estimate_->transform_with_jacobian_is_defined()) {
    if (transform_with_jacobian_ != estimate_->transform_with_jacobian()) {
      LOG_ERROR_P(PARAM_TRANSFORM_WITH_JACOBIAN) << "This parameter is not consistent with the equivalent parameter in the @estimate block "
        << estimate_label_ << ". Both of these parameters should be either true or false.";
    }
  }
  original_lower_bound_ = estimate_->lower_bound();
  original_upper_bound_ = estimate_->upper_bound();

  lower_bound_ = sqrt(original_lower_bound_);
  upper_bound_ = sqrt(original_upper_bound_);

}

/**
 *
 */
void SquareRoot::DoTransform() {
  LOG_TRACE();
  estimate_->set_lower_bound(lower_bound_);
  estimate_->set_upper_bound(upper_bound_);
  current_untransformed_value_ = estimate_->value();
  estimate_->set_value(sqrt(current_untransformed_value_));
  LOG_MEDIUM() << "Transforming value from " << current_untransformed_value_ << " to " << estimate_->value();
}

/**
 *
 */
void SquareRoot::DoRestore() {
  LOG_TRACE();
  estimate_->set_lower_bound(original_lower_bound_);
  estimate_->set_upper_bound(original_upper_bound_);
  Double check = estimate_->value() * estimate_->value();
  LOG_MEDIUM() << "Restoring value from " << estimate_->value()  << " to " << AS_DOUBLE(check);
  estimate_->set_value(estimate_->value() * estimate_->value());

}

/**
 * This method will check if the estimate needs to be transformed for the objective function. If it does then
 * it'll do the transformation.
 */
void SquareRoot::TransformForObjectiveFunction() {
  if (estimate_->transform_for_objective())
    Transform();
}

/**
 * This method will check if the estimate needs to be Restored from the objective function. If it does then
 * it'll do the undo the transformation.
 */
void SquareRoot::RestoreFromObjectiveFunction() {
  if (estimate_->transform_for_objective())
    Restore();
}

Double SquareRoot::GetScore() {
  if(transform_with_jacobian_) {
    jacobian_ = -0.5 * pow(current_untransformed_value_,-1.5);
    LOG_MEDIUM() << "Jacobian: " << jacobian_;
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
std::set<string> SquareRoot::GetTargetEstimates() {
  set<string> result;
  result.insert(estimate_label_);
  return result;
}

} /* namespace estimatetransformations */
} /* namespace niwa */
