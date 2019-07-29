/**
 * @file Simplex.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 8, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 */

// headers
#include "Simplex.h"

#include "Model/Model.h"
#include "Model/Objects.h"
#include "Model/Managers.h"
#include "Model/Factory.h"
#include "Estimates/Manager.h"
#include "Estimates/Estimate.h"
#include "Estimates/Common/Uniform.h"

// namespaces
namespace niwa {
namespace estimatetransformations {
namespace utils = niwa::utilities;
/**
 * Default constructor
 */
Simplex::Simplex(Model* model) : EstimateTransformation(model) {
  //parameters_.Bind<string>(PARAM_ESTIMATE, &estimate_label_, "The label for the estimate label to use in the simplex transformation", "");
  parameters_.Bind<Double>(PARAM_UPPER_BOUND, &upper_bound_, "The empirical upper bound for the simplex transformed, in theory it should be Inf but some of our minimisers won't allow that", "", true);
  parameters_.Bind<Double>(PARAM_LOWER_BOUND, &lower_bound_, "The empirical lower bound for the simplex transformed, in theory it should be -Inf but some of our minimisers won't allow that", "", true);
  is_simple_ = false;
}

/**
 */
void Simplex::DoValidate() {

}

/**
 *
 */
void Simplex::DoBuild() {
  LOG_TRACE();
  estimate_ = model_->managers().estimate()->GetEstimateByLabel(estimate_label_);
  if (estimate_ == nullptr) {
    LOG_ERROR_P(PARAM_ESTIMATE) << "Estimate " << estimate_label_ << " could not be found. Have you defined it?";
    return;
  }
  // Initialise for -r runs
  current_untransformed_value_ = estimate_->value();

  LOG_FINE() << "transform with objective = " << transform_with_jacobian_ << " estimeate transform " << estimate_->transform_for_objective() << " together = " << !transform_with_jacobian_ && !estimate_->transform_for_objective();
  if (!transform_with_jacobian_ && !estimate_->transform_for_objective()) {
    LOG_ERROR_P(PARAM_TRANSFORM_WITH_JACOBIAN) << "You have specified a transformation that does not contribute a jacobian, and the prior parameters do not refer to the transformed estimate, in the @estimate" << estimate_label_ << ". This is not advised, and may cause bias estimation. Please address the user manual if you need help";
  }
  if (estimate_->transform_with_jacobian_is_defined()) {
    if (transform_with_jacobian_ != estimate_->transform_with_jacobian()) {
      LOG_ERROR_P(PARAM_TRANSFORM_WITH_JACOBIAN) << "This parameter is not consistent with the equivalent parameter in the @estimate block " << estimate_label_ << ". please make sure these are both true or both false.";
    }
  }
  LOG_WARNING() << "Simplex transforamtion works but is version 1.0 may need more work for calculating bounds";
  estimates_ = model_->managers().estimate()->GetEstimatesByLabel(estimate_label_);
  if (estimates_.size() < 1) {
    LOG_ERROR_P(PARAM_ESTIMATE) << "Estimate " << estimate_label_ << " could not be found. Have you defined it?";
    return;
  }

  // Validate
  length_ = estimates_.size();
  unit_vector_.assign(length_, 0.0);
  zk_.assign(length_ - 1, 0.0);
  yk_.assign(length_ - 1, 0.0);

  // Check that the vector sums or averages to one
  for (unsigned i = 0; i < estimates_.size(); ++i) {
    total_ += estimates_[i]->value();
    estimates_[i]->set_lower_bound(lower_bound_);
    estimates_[i]->set_upper_bound(upper_bound_);

  }
  LOG_MEDIUM() << "total = " << total_ << " length " << length_;

  // Do a validation if vector doesn't sum to 1 of length (number of elements) error can't use this method
  if (fabs(1.0 - total_) > 0.0001 && (total_ - (length_)) > 0.0001)
   LOG_ERROR_P(PARAM_ESTIMATE) << "This transformation can only be used on parameters that have a mean = 1.0 or sum = 1.0";

  for (unsigned i = 0; i < unit_vector_.size(); ++i) {
    unit_vector_[i] = estimates_[i]->value() / total_ ;
  }

  LOG_MEDIUM() << "sub_total = " << sub_total_;
  // is it a true simplex or an average simplex. They differ in how many post transformation we need to do before the model can have the parameters back.
  if (fabs(1.0 - total_) < 0.0001)
    unit_ = true;

}

void Simplex::DoTransform() {
  LOG_TRACE();
  for (unsigned i = 0; i < unit_vector_.size(); ++i) {
    if (!unit_)
      unit_vector_[i] = estimates_[i]->value() / total_ ;
    else
      unit_vector_[i] = estimates_[i]->value();
    if (i < (unit_vector_.size() - 1))
      sub_total_ += unit_vector_[i];
  }
  // generate zk
  for (unsigned i = 0; i < (unit_vector_.size() - 1); ++i) {
    zk_[i] = unit_vector_[i] / (1 - sub_total_);
  }
  // generate yk
  Double count = 1.0;
  for (unsigned i = 0; i < zk_.size(); ++i) {
    yk_[i] =log(zk_[i] / (1.0 - zk_[i])) - log(1.0 / ((Double)length_ - count));
    count += 1.0;
  }
  // Set estimates, turn off the last one.
  for (unsigned i = 0; i < estimates_.size(); ++i) {
    if (i < (unit_vector_.size() - 1)) {
      // Turn off the last estimate
      estimates_[i]->set_in_objective_function(true);
      estimates_[i]->set_estimated(false);
    } else {
      estimates_[i]->set_value(yk_[i]);
    }
  }

}
/**
 *    This will restore values provided by the minimiser that need to be restored for use in the annual cycle
 */
void Simplex::DoRestore() {
  LOG_TRACE();
  // Create zk
  Double count = 1.0;
  for (unsigned i = 0; i < zk_.size(); ++i) {
    zk_[i] = 1.0 / (1.0 + exp(-yk_[i] + log(1.0 / ((Double)length_ - count))));
    count += 1.0;
  }
  // translate back to unit simplex.
  for (unsigned i = 0; i < zk_.size(); ++i) {
    ////////////////////////////// Up to here //////////////////////////////
    unit_vector_[i] = 0.0;
  }

}

/**
 *  Calculate the Jacobian, to offset the bias of the transformation that enters the objective function
 */
Double Simplex::GetScore() {
  jacobian_ = unit_vector_[0] * (1 - unit_vector_[0]);
  if (unit_vector_.size() > 1 ) {
  for (unsigned i = 1; i < unit_vector_.size(); ++i) {
    LOG_MEDIUM() << "val = " << unit_vector_[i];
    jacobian_ *= unit_vector_[i] * (1 - unit_vector_[i]);
    }
  }
  jacobian_ *= (total_ - sub_total_);
  LOG_MEDIUM() << "Jacobian: " << jacobian_;
  return jacobian_;
}

/**
 * Get the target addressables so we can ensure each
 * object is not referencing multiple ones as this would
 * cause chain issues
 *
 * @return Set of addressable labels
 */
std::set<string> Simplex::GetTargetEstimates() {
  set<string> result;
  result.insert(estimate_label_);
  return result;
}
} /* namespace estimatetransformations */
} /* namespace niwa */
