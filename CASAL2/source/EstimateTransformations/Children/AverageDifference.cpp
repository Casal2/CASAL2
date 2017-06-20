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
  parameters_.Bind<string>(PARAM_DIFFERENCE_ESTIMATE, &difference_estimate_label_, "The label from the @estimate block relating to the difference parameter", "");
  parameters_.Bind<Double>(PARAM_AVERAGE_UPPER_BOUND, &average_upper_bound_, "Upper bound for the average transformed parameter", "");
  parameters_.Bind<Double>(PARAM_AVERAGE_LOWER_BOUND, &average_lower_bound_, "Lower bound for the average transformed parameter", "");
  parameters_.Bind<Double>(PARAM_DIFF_UPPER_BOUND, &difference_upper_bound_, "Upper bound for the difference transformed parameter", "");
  parameters_.Bind<Double>(PARAM_DIFF_LOWER_BOUND, &difference_lower_bound_, "Lower bound for the difference transformed parameter", "");
  is_simple_ = false;
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
  difference_estimate_ = model_->managers().estimate()->GetEstimateByLabel(difference_estimate_label_);

  if (difference_estimate_ == nullptr) {
    LOG_ERROR_P(PARAM_DIFFERENCE_ESTIMATE) << "Estimate " << difference_estimate_label_ << " could not be found. Have you defined it?";
    return;
  }

  average_original_upper_bound_ =  estimate_->upper_bound();
  average_original_lower_bound_ =  estimate_->lower_bound();
  difference_original_upper_bound_ =  difference_estimate_->upper_bound();
  difference_original_lower_bound_ =  difference_estimate_->lower_bound();

}

/**
 *
 */
void AverageDifference::Transform() {
  x1_ = estimate_->value();
  Double mean = (x1_ + difference_estimate_->value()) / 2.0;
  Double diff = fabs(mean - x1_);

  // Set the first estimate as the mean and the second as the difference
  LOG_MEDIUM() << "Changing @estimate " << estimate_->label() << " from: " << estimate_->value() << "to: " << mean;
  LOG_MEDIUM() << "Changing @estimate " << difference_estimate_->label() << " from: " << difference_estimate_->value() << "to: " << diff;

  estimate_->set_value(mean);
  difference_estimate_->set_value(diff);

  // Set the bounds
  estimate_->set_lower_bound(average_lower_bound_);
  estimate_->set_upper_bound(average_upper_bound_);
  difference_estimate_->set_lower_bound(difference_lower_bound_);
  difference_estimate_->set_upper_bound(difference_upper_bound_);
}

/**
 *
 */
void AverageDifference::Restore() {
	x1_ = estimate_->value() + (difference_estimate_->value() / 2.0);
	difference_estimate_->set_value(estimate_->value() - (difference_estimate_->value() / 2.0));
	estimate_->set_value(x1_);
  // Restore the bounds
  estimate_->set_lower_bound(average_original_lower_bound_);
  estimate_->set_upper_bound(average_original_upper_bound_);
  difference_estimate_->set_lower_bound(difference_original_lower_bound_);
  difference_estimate_->set_upper_bound(difference_original_upper_bound_);
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

} /* namespace estimatetransformations */
} /* namespace niwa */
