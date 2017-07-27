/*
 * EstimateTransformation.cpp
 *
 *  Created on: Dec 7, 2015
 *      Author: Zaita
 */

#include "EstimateTransformation.h"

#include "Common/Estimates/Manager.h"
#include "Common/Model/Managers.h"

namespace niwa {

/**
 * Default constructor
 *
 * Bind any parameters that are allowed to be loaded from the configuration files.
 * Set bounds on registered parameters
 * Register any parameters that can be an estimated or utilised in other run modes (e.g profiling, yields, projections etc)
 * Set some initial values
 *
 * Note: The constructor is parsed to generate Latex for the documentation.
 */
EstimateTransformation::EstimateTransformation(Model* model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label for the transformation block", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type of transformation", "");
  parameters_.Bind<string>(PARAM_ESTIMATE_LABEL, &estimate_label_, "Label of estimate block to apply transformation to", "");
  parameters_.Bind<bool>(PARAM_TRANSFORM_WITH_JACOBIAN, &transform_with_jacobian_, "Apply jacobian during transformation", "", true);
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when we cannot use bind<>() overloads
 *
 * Note: all parameters are populated from configuration files
 */
void EstimateTransformation::Validate() {
  parameters_.Populate();
  DoValidate();
}

/**
 * Build relationships
 */
void EstimateTransformation::Build() {
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
 DoBuild();
}

/**
 * This method will check to ensure we're not already transformed, then transform if it's
 * ok.
 */
void EstimateTransformation::Transform() {
  LOG_TRACE();
  if (!is_transformed_) {
    DoTransform();
    is_transformed_ = true;
  } else {
    LOG_FINE() << "The transformation " << label() << " is already transformed. Skipping transform";
  }
}

/**
 * This method will check to ensure we're currently transformed, then restore
 */
void EstimateTransformation::Restore() {
  LOG_TRACE();
  if (is_transformed_) {
    DoRestore();
    is_transformed_ = false;
  } else {
    LOG_FINE() << "The transformation " << label() << " is not transformed. Skipping restore";
  }
}

}
/* namespace niwa */
