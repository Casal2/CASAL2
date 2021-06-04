/**
 * @file AverageDifference.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 8, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 */

// headers
#include "AverageDifference.h"

#include "../../Estimates/Estimate.h"
#include "../../Estimates/Manager.h"
#include "../../Model/Managers.h"
#include "../../Model/Model.h"
#include "../../Model/Objects.h"

// namespaces
namespace niwa {
namespace estimatetransformations {

/**
 * Default constructor
 */
AverageDifference::AverageDifference(shared_ptr<Model> model) : EstimateTransformation(model) {
  parameters_.Bind<string>(PARAM_THETA_TWO, &difference_estimate_label_,
                           "The label of the @estimate block relating to the $\theta_2$ parameter in the transformation. See the User Manual for more information", "");
  parameters_.Bind<string>(PARAM_THETA_ONE, &estimate_label_,
                           "The label of @estimate block relating to the $\theta_1$ parameter in the transformation. See the User Manual for more information", "");
  is_simple_ = false;
}

/**
 * Validate objects
 */
void AverageDifference::DoValidate() {
  if (transform_with_jacobian_) {
    LOG_ERROR_P(PARAM_TRANSFORM_WITH_JACOBIAN) << "The Jacobian transformation has not been worked out (if it exists) for the average difference transformation.";
  }
}

/**
 * Build objects
 */
void AverageDifference::DoBuild() {
  LOG_TRACE();
  estimate_ = model_->managers()->estimate()->GetEstimateByLabel(estimate_label_);
  if (estimate_ == nullptr) {
    LOG_ERROR_P(PARAM_THETA_ONE) << "Estimate " << estimate_label_ << " was not found.";
    return;
  }
  // Initialise for -r runs
  current_untransformed_value_ = estimate_->value();

  LOG_FINE() << "transform with objective = " << transform_with_jacobian_ << " estimate transform " << estimate_->transform_for_objective()
             << " together = " << !transform_with_jacobian_
      && !estimate_->transform_for_objective();
  if (!transform_with_jacobian_ && !estimate_->transform_for_objective()) {
    LOG_ERROR_P(PARAM_TRANSFORM_WITH_JACOBIAN) << "A transformation that does not contribute to the Jacobian was specified,"
                                               << " and the prior parameters do not refer to the transformed estimate, in the @estimate" << estimate_label_
                                               << ". This is not advised, and may cause bias errors. Please check the User Manual for more info";
  }
  if (estimate_->transform_with_jacobian_is_defined()) {
    if (transform_with_jacobian_ != estimate_->transform_with_jacobian()) {
      LOG_ERROR_P(PARAM_TRANSFORM_WITH_JACOBIAN) << "This parameter is not consistent with the equivalent parameter in the @estimate block " << estimate_label_
                                                 << ". Both parameters should be true or false.";
    }
  }
  difference_estimate_ = model_->managers()->estimate()->GetEstimateByLabel(difference_estimate_label_);

  if (difference_estimate_ == nullptr) {
    LOG_ERROR_P(PARAM_THETA_TWO) << "Estimate " << difference_estimate_label_ << " could not be found. Have you defined it?";
    return;
  }

  if ((difference_estimate_->transform_for_objective() && !estimate_->transform_for_objective())
      || (!difference_estimate_->transform_for_objective() && estimate_->transform_for_objective()))
    LOG_ERROR_P(PARAM_THETA_TWO) << "This transformation requires that both parameters have transform_for_objective either true or false";

  // check transformation is within bounds;
  if (difference_estimate_->transform_for_objective()) {
    LOG_MEDIUM() << "Check diff bounds";
    difference_original_upper_bound_ = difference_estimate_->upper_bound();
    difference_original_lower_bound_ = difference_estimate_->lower_bound();
    Transform();
    if (difference_estimate_->value() < difference_original_lower_bound_ || difference_estimate_->value() > difference_original_upper_bound_)
      LOG_ERROR_P(PARAM_THETA_TWO) << " " << PARAM_PRIOR_APPLIES_TO_TRANSFORM << " true was specified, but the transformed parameter = " << difference_estimate_->value()
                                   << " which is outside of these bounds. Please check the bounds."
                                   << " If this is on M try staggering the starting values of males and females. If they are the same value it will make the diff == 0.";
    Restore();
  }

  if (estimate_->transform_for_objective()) {
    LOG_MEDIUM() << "Check avg bounds";
    average_original_upper_bound_ = estimate_->upper_bound();
    average_original_lower_bound_ = estimate_->lower_bound();
    DoTransform();
    if (estimate_->value() < average_original_lower_bound_ || estimate_->value() > average_original_upper_bound_)
      LOG_ERROR_P(PARAM_THETA_ONE) << " " << PARAM_PRIOR_APPLIES_TO_TRANSFORM << " true was specified, but the transformed parameter = " << estimate_->value()
                                   << " which is outside of these bounds. Please check the bounds.";
    DoRestore();
  }
}

/**
 * Transform objects
 */
void AverageDifference::DoTransform() {
  if (first_time_transform_) {
    LOG_MEDIUM() << "Applying Transformation";
    x1_ = estimate_->value();
    LOG_MEDIUM() << "x1 = " << x1_ << " diff = " << difference_estimate_->value();
    Double mean = (x1_ + difference_estimate_->value()) / 2.0;
    Double diff = (mean - difference_estimate_->value()) * 2.0;

    // Set the first estimate as the mean and the second as the difference
    LOG_MEDIUM() << "Transforming @estimate " << estimate_->label() << " from: " << estimate_->value() << " to: " << mean;
    LOG_MEDIUM() << "Transforming @estimate " << difference_estimate_->label() << " from: " << difference_estimate_->value() << " to: " << diff;

    estimate_->set_value(mean);
    difference_estimate_->set_value(diff);
    first_time_transform_ = false;
  } else {
    estimate_->set_value(y1_);
    difference_estimate_->set_value(y2_);
  }
}

/**
 * Restore objects
 */
void AverageDifference::DoRestore() {
  LOG_MEDIUM() << "Restoring value";
  y1_ = estimate_->value();
  y2_ = difference_estimate_->value();

  x1_ = estimate_->value() + (difference_estimate_->value() / 2.0);
  difference_estimate_->set_value(estimate_->value() - (difference_estimate_->value() / 2.0));
  estimate_->set_value(x1_);

  // Set the first estimate as the mean and the second as the difference
  LOG_MEDIUM() << "Restoring @estimate " << estimate_->label() << " to: " << estimate_->value();
  LOG_MEDIUM() << "Restoring @estimate " << difference_estimate_->label() << " to: " << difference_estimate_->value();
}

/**
 * Get the target addressable to ensure that each
 * object is not referencing multiple ones as this would
 * cause chain issues
 *
 * @return Set of addressable labels
 */

std::set<string> AverageDifference::GetTargetEstimates() {
  set<string> result;
  result.insert(estimate_label_);
  result.insert(difference_estimate_label_);

  return result;
}

/**
 * This method will check if the estimate needs to be transformed for the objective function. If it does then
 * it will do the transformation.
 */
void AverageDifference::TransformForObjectiveFunction() {
  if (estimate_->transform_for_objective())
    Transform();
}

/**
 * This method will check if the estimate needs to be Restored from the objective function. If it does then
 * it will do the undo the transformation.
 */
void AverageDifference::RestoreFromObjectiveFunction() {
  if (estimate_->transform_for_objective())
    Restore();
}

} /* namespace estimatetransformations */
} /* namespace niwa */
