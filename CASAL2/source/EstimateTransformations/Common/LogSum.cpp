/**
 * @file LogSum.cpp
 * @author C.Marsh
 * @github https://github.com/Zaita
 * @date Jul 8, 2017
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 *
 */

// headers
#include "LogSum.h"

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
LogSum::LogSum(Model* model) : EstimateTransformation(model) {
  parameters_.Bind<string>(PARAM_THETA_TWO, &second_estimate_label_, "The label of the @estimate block relating to the $\theta_2$ parameter in the transformation. See the User Manual for more information", "");
  parameters_.Bind<string>(PARAM_THETA_ONE, &estimate_label_, "The label of @estimate block relating to the $\theta_1$ parameter in the transformation. See the User Manual for more information", "");
  is_simple_ = false;
}


/**
 * Validate objects
 */
void LogSum::DoValidate() {

}

/**
 * Build objects
 */
void LogSum::DoBuild() {
  LOG_TRACE();
  second_estimate_ = model_->managers().estimate()->GetEstimateByLabel(second_estimate_label_);
  estimate_ = model_->managers().estimate()->GetEstimateByLabel(estimate_label_);
  if (estimate_ == nullptr) {
    LOG_ERROR_P(PARAM_THETA_ONE) << "Estimate " << estimate_label_ << " was not found.";
    return;
  }

  // Initialise for -r runs
  current_untransformed_value_ = estimate_->value();

  LOG_FINE() << "transform with objective = " << transform_with_jacobian_ << " estimate transform "
    << estimate_->transform_for_objective() << " together = " << !transform_with_jacobian_ && !estimate_->transform_for_objective();

  if (!transform_with_jacobian_ && !estimate_->transform_for_objective()) {
    LOG_ERROR_P(PARAM_TRANSFORM_WITH_JACOBIAN) << "A transformation that does not contribute to the Jacobian was specified,"
      << " and the prior parameters do not refer to the transformed estimate, in the @estimate" << estimate_label_
      << ". This is not advised, and may cause bias errors. Please check the User Manual for more info";
  }

  if (estimate_->transform_with_jacobian_is_defined()) {
    if (transform_with_jacobian_ != estimate_->transform_with_jacobian()) {
      LOG_ERROR_P(PARAM_TRANSFORM_WITH_JACOBIAN) << "This parameter is not consistent with the equivalent parameter in the @estimate block "
        << estimate_label_ << ". Both of these parameters should be true or false.";
    }
  }

  if (!second_estimate_) {
    LOG_ERROR_P(PARAM_THETA_TWO) << "Estimate " << second_estimate_label_ << " was not found.";
    return;
  }

  if ( (second_estimate_->transform_for_objective() && !estimate_->transform_for_objective()) ||
       (!second_estimate_->transform_for_objective() && estimate_->transform_for_objective()) )
    LOG_ERROR_P(PARAM_THETA_TWO) << "This transformation requires that both parameters set 'transform_for_objective' to true or false";

  // check transformation is within bounds;
  if (second_estimate_->transform_for_objective()) {
    Transform();
    LOG_MEDIUM() << "Check second param bounds";
    if(second_estimate_->value() < second_estimate_->lower_bound() || second_estimate_->value() > second_estimate_->upper_bound())
      LOG_ERROR_P(PARAM_THETA_TWO) << " " << PARAM_PRIOR_APPLIES_TO_TRANSFORM << " was set to true, but the transformed parameter = "
        << second_estimate_->value() << " which is outside of these bounds. Please check the bounds.";
    LOG_MEDIUM() << "Check first param bounds";
    if(estimate_->value() < estimate_->lower_bound() || estimate_->value() >  estimate_->upper_bound())
      LOG_ERROR_P(PARAM_THETA_ONE) << " " << PARAM_PRIOR_APPLIES_TO_TRANSFORM << " was set to true, but the transformed parameter = "
        << estimate_->value() << " which is outside of these bounds. Please check the bounds.";
    Restore();
  }
}

/**
 * Transform objects
 */
void LogSum::DoTransform() {
  LOG_MEDIUM() << "Applying Transformation";
  xt_ = estimate_->value() + second_estimate_->value();
  Double prop = estimate_->value() / xt_;

  // Set the first estimate as the mean and the second as the difference
  LOG_MEDIUM() << "Transforming @estimate " << estimate_->label() << " from: " << estimate_->value() << " to: " << AS_DOUBLE(log(xt_));
  LOG_MEDIUM() << "Transforming @estimate " << second_estimate_->label() << " from: " << second_estimate_->value()
    << " to: " << AS_DOUBLE(prop);

  estimate_->set_value(log(xt_));
  second_estimate_->set_value(prop);
}

/**
 * Restore objects
 */
void LogSum::DoRestore() {
  LOG_MEDIUM() << "Restoring value";
  xt_ = exp(estimate_->value());
  estimate_->set_value(xt_ * second_estimate_->value());
  second_estimate_->set_value(xt_ * (1 - second_estimate_->value()));

  // Set the first estimate as the mean and the second as the difference
  LOG_MEDIUM() << "Restoring @estimate " << estimate_->label() << "to: " << estimate_->value();
  LOG_MEDIUM() << "Restoring @estimate " << second_estimate_->label() << "to: " << second_estimate_->value();
}

/**
 * Get the target addressable so we can ensure each
 * object is not referencing multiple ones as this would
 * cause chain issues
 *
 * @return Set of addressable labels
 */
std::set<string> LogSum::GetTargetEstimates() {
  set<string> result;
  result.insert(estimate_label_);
  result.insert(second_estimate_label_);

  return result;
}

/**
 * This method will check if the estimate needs to be transformed for the objective function. If it does then
 * it'll do the transformation.
 */
void LogSum::TransformForObjectiveFunction() {
  if (estimate_->transform_for_objective())
    Transform();
}

/**
 * This method will check if the estimate needs to be Restored from the objective function. If it does then
 * it'll do the undo the transformation.
 */
void LogSum::RestoreFromObjectiveFunction() {
  if (estimate_->transform_for_objective())
    Restore();
}

} /* namespace estimatetransformations */
} /* namespace niwa */
