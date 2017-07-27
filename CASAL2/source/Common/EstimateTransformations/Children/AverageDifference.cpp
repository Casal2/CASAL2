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

#include "Common/Model/Model.h"
#include "Common/Model/Objects.h"
#include "Common/Model/Managers.h"
#include "Common/Estimates/Manager.h"
#include "Common/Estimates/Estimate.h"

// namespaces
namespace niwa {
namespace estimatetransformations {

/**
 * Default constructor
 */
AverageDifference::AverageDifference(Model* model) : EstimateTransformation(model) {
  parameters_.Bind<string>(PARAM_DIFFERENCE_ESTIMATE, &difference_estimate_label_, "The label from the @estimate block relating to the difference parameter, used in this transformation", "");
  is_simple_ = false;
}

/**
 */
void AverageDifference::DoValidate() {
  if (transform_with_jacobian_) {
    LOG_ERROR_P(PARAM_TRANSFORM_WITH_JACOBIAN) << "The jacobian transformation has not been worked out (if it exists) for the average difference transformation, If you would like to do by all means get stuck in.";
  }

}

/**
 *
 */
void AverageDifference::DoBuild() {
  LOG_TRACE();
  LOG_MEDIUM() << "DoBuild";
  difference_estimate_ = model_->managers().estimate()->GetEstimateByLabel(difference_estimate_label_);

  if (difference_estimate_ == nullptr) {
    LOG_ERROR_P(PARAM_DIFFERENCE_ESTIMATE) << "Estimate " << difference_estimate_label_ << " could not be found. Have you defined it?";
    return;
  }

  if ((difference_estimate_->transform_for_objective() & !estimate_->transform_for_objective()) || (!difference_estimate_->transform_for_objective() & estimate_->transform_for_objective()))
    LOG_ERROR_P(PARAM_DIFFERENCE_ESTIMATE) << "This transformation requires that both parameters have transform_for_objective either true or false not one true and the other and vice versa";
  // check transformation is within bounds;
  if (difference_estimate_->transform_for_objective()) {
    LOG_MEDIUM() << "Check diff bounds";
    difference_original_upper_bound_ =  difference_estimate_->upper_bound();
    difference_original_lower_bound_ =  difference_estimate_->lower_bound();
    Transform();
    if(difference_estimate_->value() < difference_original_lower_bound_ || difference_estimate_->value() > difference_original_upper_bound_)
      LOG_ERROR_P(PARAM_DIFFERENCE_ESTIMATE) << "You have specified " << PARAM_PRIOR_APPLIES_TO_TRANSFORM << " true, but the transformed parameter = " << difference_estimate_->value() << " which is outside one of these bounds, please reconsider your choice in bounds. If this is on M try staggering the starting values of males and females if they are the same it will make the diff == 0.";
    Restore();
  }

  if (estimate_->transform_for_objective()) {
    LOG_MEDIUM() << "Check avg bounds";
    average_original_upper_bound_ =  estimate_->upper_bound();
    average_original_lower_bound_ =  estimate_->lower_bound();
    DoTransform();
    if(estimate_->value() < average_original_lower_bound_ || estimate_->value() > average_original_upper_bound_)
      LOG_ERROR_P(PARAM_ESTIMATE) << "You have specified " << PARAM_PRIOR_APPLIES_TO_TRANSFORM << " true, but the transformed parameter = " << estimate_->value() << " which is outside one of these bounds, please reconsider your choice in bounds.";
    DoRestore();
  }
}

/**
 *
 */
void AverageDifference::DoTransform() {
  LOG_MEDIUM() << "Applying Transformation";
  x1_ = estimate_->value();
  LOG_MEDIUM() << "x1 = " << x1_ << " diff = " << difference_estimate_->value();
  Double mean = (x1_ + difference_estimate_->value()) / 2.0;
  Double diff = fabs(mean - x1_) * 2.0;

  // Set the first estimate as the mean and the second as the difference
  LOG_MEDIUM() << "Transforming @estimate " << estimate_->label() << " from: " << estimate_->value() << "to: " << mean;
  LOG_MEDIUM() << "Transforming @estimate " << difference_estimate_->label() << " from: " << difference_estimate_->value() << "to: " << diff;

  estimate_->set_value(mean);
  difference_estimate_->set_value(diff);

/*
  // Set the bounds
  estimate_->set_lower_bound(average_lower_bound_);
  estimate_->set_upper_bound(average_upper_bound_);
  difference_estimate_->set_lower_bound(difference_lower_bound_);
  difference_estimate_->set_upper_bound(difference_upper_bound_);
*/

}

/**
 *
 */
void AverageDifference::DoRestore() {
  LOG_MEDIUM() << "Restoring value";
	x1_ = estimate_->value() + (difference_estimate_->value() / 2.0);
	difference_estimate_->set_value(estimate_->value() - (difference_estimate_->value() / 2.0));
	estimate_->set_value(x1_);
/*
  // Restore the bounds
  estimate_->set_lower_bound(average_original_lower_bound_);
  estimate_->set_upper_bound(average_original_upper_bound_);
  difference_estimate_->set_lower_bound(difference_original_lower_bound_);
  difference_estimate_->set_upper_bound(difference_original_upper_bound_);
*/
  // Set the first estimate as the mean and the second as the difference
  LOG_MEDIUM() << "Restoring @estimate " << estimate_->label() << "to: " << estimate_->value();
  LOG_MEDIUM() << "Restoring @estimate " << difference_estimate_->label() << "to: " << difference_estimate_->value();
}

/**
 * Get the target addressable so we can ensure each
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
 * it'll do the transformation.
 */
void AverageDifference::TransformForObjectiveFunction() {
  if (estimate_->transform_for_objective())
    Transform();
}

/**
 * This method will check if the estimate needs to be Restored from the objective function. If it does then
 * it'll do the undo the transformation.
 */
void AverageDifference::RestoreFromObjectiveFunction() {
  if (estimate_->transform_for_objective())
    Restore();
}

} /* namespace estimatetransformations */
} /* namespace niwa */
