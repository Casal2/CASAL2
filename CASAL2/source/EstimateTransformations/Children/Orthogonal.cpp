/**
 * @file Orthogonal.cpp
 * @author C.Marsh
 * @date 13/04/2017
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 *
 */

// headers
#include "Orthogonal.h"

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
Orthogonal::Orthogonal(Model* model) : EstimateTransformation(model) {
  parameters_.Bind<string>(PARAM_FIRST_ESTIMATE, &first_estimate_label_, "$\theta_1$ The first variable to use in the transformation", "");
  parameters_.Bind<string>(PARAM_SECOND_ESTIMATE, &second_estimate_label_, "$\theta_2$ The second variable to use in the transformation", "");

  is_simple_ = false;
}

/**
 */
void Orthogonal::DoValidate() {

}

/**
 *
 */
void Orthogonal::DoBuild() {
  LOG_TRACE();
  first_estimate_ = model_->managers().estimate()->GetEstimateByLabel(first_estimate_label_);
  second_estimate_ = model_->managers().estimate()->GetEstimateByLabel(second_estimate_label_);

  if (first_estimate_ == nullptr) {
    LOG_ERROR_P(PARAM_FIRST_ESTIMATE) << "Estimate " << first_estimate_label_ << " could not be found. Have you defined it in an @estimate block?";
    return;
  }
  if (second_estimate_ == nullptr) {
    LOG_ERROR_P(PARAM_SECOND_ESTIMATE) << "Estimate " << second_estimate_label_ << " could not be found. Have you defined it in an @estimate block?";
    return;
  }
  first_original_upper_bound_ =  first_estimate_->upper_bound();
  first_original_lower_bound_ =  first_estimate_->lower_bound();
  second_original_upper_bound_ =  second_estimate_->upper_bound();
  second_original_lower_bound_ =  second_estimate_->lower_bound();
}

/**
 *  transform estimate
 */
void Orthogonal::Transform() {
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
  LOG_MEDIUM() << "theta 1 UB " << first_estimate_->upper_bound() << " and LB " << first_estimate_->lower_bound() << " theta 2 UB = " << second_estimate_->upper_bound() << " LB = " << second_estimate_->lower_bound();
}

/**
 *  Restore estimate
 */
void Orthogonal::Restore() {
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

Double Orthogonal::GetScore() {
  LOG_TRACE();
  jacobian_ = theta_2_ / (2 * theta_1_);
  LOG_MEDIUM() << "Jacobian contribution = " << jacobian_;
  return jacobian_;
}

/**
 * Get the target estimables so we can ensure each
 * object is not referencing multiple ones as this would
 * cause chain issues
 *
 * @return Set of estimable labels
 */
std::set<string> Orthogonal::GetTargetEstimates() {
  set<string> result;
  result.insert(first_estimate_label_);
  result.insert(second_estimate_label_);
  return result;
}
} /* namespace estimatetransformations */
} /* namespace niwa */
