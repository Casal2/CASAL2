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
  //parameters_.Bind<string>(PARAM_ESTIMATE, &estimate_label_, "The parameter to use in the inverse transformation", "");
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
  estimate_ = model_->managers().estimate()->GetEstimateByLabel(estimate_label_);
  if (estimate_ == nullptr) {
    LOG_ERROR_P(PARAM_ESTIMATE) << "Estimate " << estimate_label_ << " could not be found. Have you defined it?";
    return;
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
void Inverse::Transform() {
  estimate_->set_lower_bound(upper_bound_);
  estimate_->set_upper_bound(lower_bound_);
  current_untransformed_value_ = estimate_->value();
  estimate_->set_value(1 / current_untransformed_value_);
  LOG_MEDIUM() << "transforming value from " << current_untransformed_value_ << " to " << estimate_->value();

}

/**
 *
 */
void Inverse::Restore() {
  estimate_->set_lower_bound(original_lower_bound_);
  estimate_->set_upper_bound(original_upper_bound_);
  LOG_MEDIUM() << "Restoring value from " << estimate_->value()  << " to " << AS_DOUBLE(1.0 /  estimate_->value());

  estimate_->set_value(1.0 / estimate_->value());
}


/**
 *  @Return the jacobian
 */
Double Inverse::GetScore() {
//
  if(transform_with_jacobian_) {
    jacobian_ = -1.0 * pow(current_untransformed_value_,-2);
    LOG_MEDIUM() << "jacobian: " << jacobian_;
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
