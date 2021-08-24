/**
 * @file Orthogonal.cpp
 * @author C.Marsh
 * @date 13/04/2017
 * @section LICENSE
 *
 * Copyright NIWA Science �2017 - www.niwa.co.nz
 *
 */

// headers
#include "Orthogonal.h"

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
Orthogonal::Orthogonal(shared_ptr<Model> model) : EstimateTransformation(model) {
  parameters_.Bind<string>(PARAM_THETA_TWO, &second_estimate_label_,
                           "The label of the @estimate block relating to the $\theta_2$ parameter in the transformation. See the User Manual for more information", "");
  parameters_.Bind<string>(PARAM_THETA_ONE, &estimate_label_,
                           "The label of @estimate block relating to the $\theta_1$ parameter in the transformation. See the User Manual for more information", "");

  is_simple_ = false;
}

/**
 * Validate
 */
void Orthogonal::DoValidate() {}

/**
 * Build
 */
void Orthogonal::DoBuild() {
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
                                               << ". This is not permitted";
  }
  if (estimate_->transform_with_jacobian_is_defined()) {
    if (transform_with_jacobian_ != estimate_->transform_with_jacobian()) {
      LOG_ERROR_P(PARAM_TRANSFORM_WITH_JACOBIAN) << "This parameter is not consistent with the equivalent parameter in the @estimate block " << estimate_label_
                                                 << ". Both of these parameters should be true or false.";
    }
  }
  LOG_TRACE();
  first_estimate_  = model_->managers()->estimate()->GetEstimateByLabel(estimate_label_);
  second_estimate_ = model_->managers()->estimate()->GetEstimateByLabel(second_estimate_label_);

  if (first_estimate_ == nullptr) {
    LOG_ERROR_P(PARAM_THETA_ONE) << "Estimate " << estimate_label_ << " was not found.";
    return;
  }
  if (second_estimate_ == nullptr) {
    LOG_ERROR_P(PARAM_THETA_TWO) << "Estimate " << second_estimate_label_ << " was not found.";
    return;
  }
  first_original_upper_bound_  = first_estimate_->upper_bound();
  first_original_lower_bound_  = first_estimate_->lower_bound();
  second_original_upper_bound_ = second_estimate_->upper_bound();
  second_original_lower_bound_ = second_estimate_->lower_bound();

  theta_1_ = first_estimate_->value();
  theta_2_ = second_estimate_->value();
}

/**
 *  Transform estimate
 */
void Orthogonal::DoTransform() {
  LOG_TRACE();
  // transform
  theta_1_ = first_estimate_->value();
  theta_2_ = second_estimate_->value();

  phi_1_ = theta_1_ * theta_2_;
  phi_2_ = theta_1_ / theta_2_;

  LOG_MEDIUM() << "theta 1 = " << theta_1_ << " becomes phi 1 " << phi_1_ << " theta 2 = " << theta_2_ << " becomes phi 2 " << phi_2_;
  first_estimate_->set_value(phi_1_);
  second_estimate_->set_value(phi_2_);
  // Transform bounds
  first_estimate_->set_lower_bound(first_original_lower_bound_ * second_original_lower_bound_);
  first_estimate_->set_upper_bound(first_original_upper_bound_ * second_original_upper_bound_);
  second_estimate_->set_lower_bound(first_original_lower_bound_ / second_original_upper_bound_);
  second_estimate_->set_upper_bound(first_original_upper_bound_ / second_original_lower_bound_);
  LOG_MEDIUM() << "theta 1 UB " << first_estimate_->upper_bound() << " and LB " << first_estimate_->lower_bound() << " theta 2 UB = " << second_estimate_->upper_bound()
               << " LB = " << second_estimate_->lower_bound();
}

/**
 *  Restore estimate
 */
void Orthogonal::DoRestore() {
  LOG_TRACE();
  theta_1_ = sqrt(first_estimate_->value() * second_estimate_->value());
  theta_2_ = sqrt(first_estimate_->value() / second_estimate_->value());

  first_estimate_->set_value(theta_1_);
  second_estimate_->set_value(theta_2_);
  first_estimate_->set_lower_bound(first_original_lower_bound_);
  first_estimate_->set_upper_bound(first_original_upper_bound_);
  second_estimate_->set_lower_bound(second_original_lower_bound_);
  second_estimate_->set_upper_bound(second_original_upper_bound_);
}

/**
 * GetScore
 * @return Jacobian
 */
Double Orthogonal::GetScore() {
  LOG_TRACE();
  if (transform_with_jacobian_) {
    jacobian_ = -log(theta_2_ / (2 * theta_1_));
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
std::set<string> Orthogonal::GetTargetEstimates() {
  set<string> result;
  result.insert(estimate_label_);
  result.insert(second_estimate_label_);
  return result;
}
} /* namespace estimatetransformations */
} /* namespace niwa */
