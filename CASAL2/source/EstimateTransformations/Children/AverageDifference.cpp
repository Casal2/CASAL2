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
  parameters_.Bind<string>(PARAM_FIRST_ESTIMATE, &first_estimate_label_, "First estimate to transform", "");
  parameters_.Bind<string>(PARAM_SECOND_ESTIMATE, &second_estimate_label_, "Second estimate to transform", "");
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
  first_estimate_ = model_->managers().estimate()->GetEstimateByLabel(first_estimate_label_);
  second_estimate_ = model_->managers().estimate()->GetEstimateByLabel(second_estimate_label_);

  if (first_estimate_ == nullptr) {
    LOG_ERROR_P(PARAM_FIRST_ESTIMATE) << "Estimate " << first_estimate_label_ << " could not be found. Have you defined it?";
    return;
  }
  if (second_estimate_ == nullptr) {
    LOG_ERROR_P(PARAM_SECOND_ESTIMATE) << "Estimate " << second_estimate_label_ << " could not be found. Have you defined it?";
    return;
  }
  if (first_estimate_->value() >= second_estimate_->value())
    first_value_bigger_ = true;


//
//  original_lower_bound_ = estimate_->lower_bound();
//  original_upper_bound_ = estimate_->upper_bound();
//
//  if (!parameters_.Get(PARAM_LOWER_BOUND)->has_been_defined() && original_upper_bound_ != 0.0)
//    lower_bound_ = 1 / original_upper_bound_;
//  if (!parameters_.Get(PARAM_UPPER_BOUND)->has_been_defined() && original_lower_bound_ != 0.0)
//    upper_bound_ = 1 / original_lower_bound_;
}

/**
 *
 */
void AverageDifference::Transform() {
  LOG_WARNING() << "AverageDifference transforamtion still needs work, especially on bounds()";
  Double first_val = first_estimate_->value();
  Double mean = (first_val + second_estimate_->value()) / 2.0;
  Double diff = fabs(mean - first_val);

  // Set the first estimate as the mean and the second as the difference
  LOG_FINE() << "Changing @estimate " << first_estimate_->label() << " from: " << first_estimate_->value() << "to: " << mean;
  LOG_FINE() << "Changing @estimate " << second_estimate_->label() << " from: " << second_estimate_->value() << "to: " << diff;

  first_estimate_->set_value(mean);
  second_estimate_->set_value(diff);
}

/**
 *
 */
void AverageDifference::Restore() {
  if (first_value_bigger_) {
    first_estimate_->set_value(first_estimate_->value() + 0.5 * second_estimate_->value());
    second_estimate_->set_value(first_estimate_->value() - 0.5 * second_estimate_->value());

  } else {
    first_estimate_->set_value(first_estimate_->value() - 0.5 * second_estimate_->value());
    second_estimate_->set_value(first_estimate_->value() + 0.5 * second_estimate_->value());
  }
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
  result.insert(first_estimate_label_);
  result.insert(second_estimate_label_);

  return result;
}

} /* namespace estimatetransformations */
} /* namespace niwa */
