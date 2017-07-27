/**
 * @file LogSum.cpp
 * @author C.Marsh
 * @github https://github.com/Zaita
 * @date Jul 8, 2017
 * @section LICENSE
 *
 * Copyright NIWA Science �2017 - www.niwa.co.nz
 *
 */

// headers
#include "LogSum.h"

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
LogSum::LogSum(Model* model) : EstimateTransformation(model) {
  parameters_.Bind<string>(PARAM_SECOND_ESTIMATE, &second_estimate_label_, "The label from the @estimate block relating to the difference parameter, used in this transformation", "");
  is_simple_ = false;
}


/**
 * Validate
 */
void LogSum::DoValidate() {

}

/**
 *
 */
void LogSum::DoBuild() {
  LOG_TRACE();
  second_estimate_ = model_->managers().estimate()->GetEstimateByLabel(second_estimate_label_);

  if (!second_estimate_) {
    LOG_ERROR_P(PARAM_SECOND_ESTIMATE) << "Estimate " << second_estimate_label_ << " could not be found. Have you defined it?";
    return;
  }

  if ((second_estimate_->transform_for_objective() & !estimate_->transform_for_objective()) || (!second_estimate_->transform_for_objective() & estimate_->transform_for_objective()))
    LOG_ERROR_P(PARAM_SECOND_ESTIMATE) << "This transformation requires that both parameters have transform_for_objective either true or false not one true and the other and vice versa";
  // check transformation is within bounds;
  if (second_estimate_->transform_for_objective()) {

    Transform();
    LOG_MEDIUM() << "Check second param bounds";
    if(second_estimate_->value() < second_estimate_->lower_bound() || second_estimate_->value() > second_estimate_->upper_bound())
      LOG_ERROR_P(PARAM_SECOND_ESTIMATE) << "You have specified " << PARAM_PRIOR_APPLIES_TO_TRANSFORM << " true, but the transformed parameter = " << second_estimate_->value() << " which is outside one of these bounds, please reconsider your choice in bounds.";
    LOG_MEDIUM() << "Check first param bounds";
    if(estimate_->value() < estimate_->lower_bound() || estimate_->value() >  estimate_->upper_bound())
      LOG_ERROR_P(PARAM_ESTIMATE) << "You have specified " << PARAM_PRIOR_APPLIES_TO_TRANSFORM << " true, but the transformed parameter = " << estimate_->value() << " which is outside one of these bounds, please reconsider your choice in bounds.";
    Restore();
  }
}

/**
 *
 */
void LogSum::DoTransform() {
  LOG_MEDIUM() << "Applying Transformation";
  xt_ = estimate_->value() + second_estimate_->value();
  Double prop = estimate_->value() / xt_;

  // Set the first estimate as the mean and the second as the difference
  LOG_MEDIUM() << "Transforming @estimate " << estimate_->label() << " from: " << estimate_->value() << " to: " << AS_DOUBLE(log(xt_));
  LOG_MEDIUM() << "Transforming @estimate " << second_estimate_->label() << " from: " << second_estimate_->value() << " to: " << AS_DOUBLE(prop);

  estimate_->set_value(log(xt_));
  second_estimate_->set_value(prop);
}

/**
 *
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
