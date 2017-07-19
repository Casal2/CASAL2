/**
 * @file Inverse.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 8, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 */

// headers
#include "Inverse.h"

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
Inverse::Inverse(Model* model) : EstimateTransformation(model) {

}

/**
 */
void Inverse::DoValidate() {

}

/**
 *
 */
void Inverse::DoBuild() {
  LOG_TRACE();

  original_lower_bound_ = estimate_->lower_bound();
  original_upper_bound_ = estimate_->upper_bound();
  // tranformed bounds
  lower_bound_ = 1.0 / original_lower_bound_;
  upper_bound_ = 1.0 / original_upper_bound_;
  current_untransformed_value_ = estimate_->value();
}

/**
 *
 */
void Inverse::DoTransform() {
  estimate_->set_lower_bound(upper_bound_);
  estimate_->set_upper_bound(lower_bound_);
  current_untransformed_value_ = estimate_->value();
  estimate_->set_value(1 / current_untransformed_value_);
  LOG_MEDIUM() << "transforming value from " << current_untransformed_value_ << " to " << estimate_->value();

}

/**
 *
 */
void Inverse::DoRestore() {
  estimate_->set_lower_bound(original_lower_bound_);
  estimate_->set_upper_bound(original_upper_bound_);
  LOG_MEDIUM() << "Restoring value from " << estimate_->value()  << " to " << AS_DOUBLE(1.0 /  estimate_->value());

  estimate_->set_value(1.0 / estimate_->value());
}

/**
 * This method will check if the estimate needs to be transformed for the objective function. If it does then
 * it'll do the transformation.
 */
void Inverse::TransformForObjectiveFunction() {
  if (estimate_->transform_for_objective())
    Transform();
}

/**
 * This method will check if the estimate needs to be Restored from the objective function. If it does then
 * it'll do the undo the transformation.
 */
void Inverse::RestoreFromObjectiveFunction() {
  if (estimate_->transform_for_objective())
    Restore();
}

/**
 *  @Return the jacobian
 */
Double Inverse::GetScore() {
//
  if(transform_with_jacobian_) {
    jacobian_ = -1.0 * pow(current_untransformed_value_,-2);
    LOG_MEDIUM() << "jacobian: " << jacobian_ << " current value " << current_untransformed_value_;
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
std::set<string> Inverse::GetTargetEstimates() {
  set<string> result;
  result.insert(estimate_label_);
  return result;
}

} /* namespace estimatetransformations */
} /* namespace niwa */
