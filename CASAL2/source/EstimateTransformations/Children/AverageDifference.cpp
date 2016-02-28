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
AverageDifference::AverageDifference(Model* model) : EstimateTransformation(model) {
  parameters_.Bind<string>(PARAM_ESTIMATE, &estimate_label_, "Estimate to transform", "");
}

/**
 */
void AverageDifference::DoValidate() {

}

/**
 *
 */
void AverageDifference::DoBuild() {
  LOG_TRACE();
  estimate_ = model_->managers().estimate()->GetEstimateByLabel(estimate_label_);
  if (estimate_ == nullptr) {
    LOG_ERROR_P(PARAM_ESTIMATE) << "Estimate " << estimate_label_ << " could not be found. Have you defined it?";
    return;
  }

  original_lower_bound_ = estimate_->lower_bound();
  original_upper_bound_ = estimate_->upper_bound();

  if (!parameters_.Get(PARAM_LOWER_BOUND)->has_been_defined() && original_upper_bound_ != 0.0)
    lower_bound_ = 1 / original_upper_bound_;
  if (!parameters_.Get(PARAM_UPPER_BOUND)->has_been_defined() && original_lower_bound_ != 0.0)
    upper_bound_ = 1 / original_lower_bound_;
}

/**
 *
 */
void AverageDifference::Transform() {
  estimate_->set_lower_bound(lower_bound_);
  estimate_->set_upper_bound(upper_bound_);

  if (estimate_->value() != 0)
    estimate_->set_value(1 / estimate_->value());;
}

/**
 *
 */
void AverageDifference::Restore() {
  estimate_->set_lower_bound(original_lower_bound_);
  estimate_->set_upper_bound(original_upper_bound_);

  if (estimate_->value() != 0)
    estimate_->set_value(1 / estimate_->value());
}

/**
 * Get the target estimables so we can ensure each
 * object is not referencing multiple ones as this would
 * cause chain issues
 *
 * @return Set of estimable labels
 */
std::set<string> AverageDifference::GetTargetEstimates() {
  set<string> result;
  result.insert(estimate_label_);
  return result;
}

} /* namespace estimatetransformations */
} /* namespace niwa */
