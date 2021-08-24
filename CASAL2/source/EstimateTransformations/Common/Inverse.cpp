/**
 * @file Inverse.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 8, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science �2016 - www.niwa.co.nz
 *
 */

// headers
#include "Inverse.h"

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
Inverse::Inverse(shared_ptr<Model> model) : EstimateTransformation(model) {
  parameters_.Bind<string>(PARAM_ESTIMATE_LABEL, &estimate_label_, "The label of estimate block to apply transformation. Defined as $\theta_1$ in the documentation", "");
}

/**
 * Validate
 */
void Inverse::DoValidate() {}

/**
 * Build
 */
void Inverse::DoBuild() {
  LOG_TRACE();
  estimate_ = model_->managers()->estimate()->GetEstimateByLabel(estimate_label_);
  if (estimate_ == nullptr) {
    LOG_ERROR_P(PARAM_ESTIMATE) << "Estimate " << estimate_label_ << " was not found.";
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
                                               << ". This is not permitted";
  }
  if (estimate_->transform_with_jacobian_is_defined()) {
    if (transform_with_jacobian_ != estimate_->transform_with_jacobian()) {
      LOG_ERROR_P(PARAM_TRANSFORM_WITH_JACOBIAN) << "This parameter is not consistent with the equivalent parameter in the @estimate block " << estimate_label_
                                                 << ". Both parameters should be true or false.";
    }
  }
  original_lower_bound_ = estimate_->lower_bound();
  original_upper_bound_ = estimate_->upper_bound();
  // tranformed bounds
  lower_bound_ = 1.0 / original_lower_bound_;
  upper_bound_ = 1.0 / original_upper_bound_;
}

/**
 *
 */
void Inverse::DoTransform() {
  estimate_->set_lower_bound(upper_bound_);
  estimate_->set_upper_bound(lower_bound_);
  current_untransformed_value_ = estimate_->value();
  estimate_->set_value(1 / current_untransformed_value_);
  LOG_MEDIUM() << "Transforming value from " << current_untransformed_value_ << " to " << estimate_->value();
}

/**
 * Restore
 */
void Inverse::DoRestore() {
  estimate_->set_lower_bound(original_lower_bound_);
  estimate_->set_upper_bound(original_upper_bound_);
  LOG_MEDIUM() << "Restoring value from " << estimate_->value() << " to " << AS_DOUBLE(1.0 / estimate_->value());

  estimate_->set_value(1.0 / estimate_->value());
}

/**
 * This method will check if the estimate needs to be transformed for the objective function. If it does then
 * it will do the transformation.
 */
void Inverse::TransformForObjectiveFunction() {
  if (estimate_->transform_for_objective())
    Transform();
}

/**
 * This method will check if the estimate needs to be Restored from the objective function. If it does then
 * it will do the undo the transformation.
 */
void Inverse::RestoreFromObjectiveFunction() {
  if (estimate_->transform_for_objective())
    Restore();
}

/**
 * GetScore
 * @return Jacobian if transformed with Jacobian, otherwise 0.0
 */
Double Inverse::GetScore() {
  LOG_TRACE()
  if (transform_with_jacobian_) {
    jacobian_ = -log(-1.0 * pow(current_untransformed_value_, -2));
    LOG_MEDIUM() << "Jacobian: " << jacobian_ << " current value " << current_untransformed_value_;
  } else
    jacobian_ = 0.0;
  return jacobian_;
}

/**
 * Get the target addressables to ensure that each
 * object is not referencing multiple ones as this would
 * cause chain issues
 *
 * @return Set of addressable labels
 */
std::set<string> Inverse::GetTargetEstimates() {
  set<string> result;
  result.insert(estimate_label_);
  return result;
}

} /* namespace estimatetransformations */
} /* namespace niwa */
