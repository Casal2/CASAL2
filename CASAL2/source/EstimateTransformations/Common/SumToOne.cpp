/**
 * @file SumToOne.cpp
 * @author C.Marsh
 * @github https://github.com/
 * @date Jan/11/2018
 * @section LICENSE
 *
 * Copyright NIWA Science �2018 - www.niwa.co.nz
 *
 */

// headers
#include "SumToOne.h"

#include "../../Model/Model.h"
#include "../../Model/Objects.h"
#include "../../Model/Managers.h"
#include "../../Model/Factory.h"
#include "../../Estimates/Manager.h"
#include "../../Estimates/Estimate.h"
#include "../../Estimates/Common/Uniform.h"

// namespaces
namespace niwa {
namespace estimatetransformations {
namespace utils = niwa::utilities;
/**
 * Default constructor
 */
SumToOne::SumToOne(shared_ptr<Model> model) : EstimateTransformation(model) {
  parameters_.Bind<string>(PARAM_ESTIMATE_LABELS, &estimate_labels_, "The label for the estimates for the sum to one transformation", "");
  parameters_.Bind<Double>(PARAM_UPPER_BOUND, &upper_bounds_, "The empirical upper bounds for the transformed parameters. There should be one less bound than parameters", "", true);
  parameters_.Bind<Double>(PARAM_LOWER_BOUND, &lower_bounds_, "The empirical lower bound for the transformed parameters. There should be one less bound than parameters", "", true);
  is_simple_ = false;
}

/**
 * Validate objects
 */
void SumToOne::DoValidate() {
  LOG_TRACE();
  if (parameters_.Get(PARAM_UPPER_BOUND)->has_been_defined() && parameters_.Get(PARAM_LOWER_BOUND)->has_been_defined()) {
    if (estimate_labels_.size() > 2)
      LOG_WARNING() << "This transformation was defined with two parameters. Be cautious using this transformation with more than two parameters.";

    if (upper_bounds_.size() != lower_bounds_.size())
      LOG_ERROR_P(PARAM_LOWER_BOUND) << "Supply the same number of upper and lower bounds. '" << estimate_labels_.size() << "' estimate labels and '"
        << lower_bounds_.size() << "' bound values were parsed.";

    if ((estimate_labels_.size() - 1) != lower_bounds_.size())
      LOG_ERROR_P(PARAM_ESTIMATE_LABELS) << "Supply one less bound than estimate labels. '" << upper_bounds_.size() << "' upper bound values and '"
        << lower_bounds_.size() << "' lower bound values were parsed.";
  }
}

/**
 * Build objects
 */
void SumToOne::DoBuild() {
  LOG_TRACE();
  for (auto& estimate_label : estimate_labels_) {
    Estimate* estimate = model_->managers()->estimate()->GetEstimateByLabel(estimate_label);
    if (estimate == nullptr) {
      LOG_ERROR_P(PARAM_ESTIMATE_LABELS) << "Estimate " << estimate_label << " was not found.";
      return;
    } else {
      LOG_FINE() << "transform with objective = " << transform_with_jacobian_ << " estimate transform " << estimate->transform_for_objective()
        << " together = " << !transform_with_jacobian_ && !estimate->transform_for_objective();

      if (!transform_with_jacobian_ && !estimate->transform_for_objective()) {
        LOG_ERROR_P(PARAM_LABEL) << "The specified transformation does not contribute to the Jacobian matrix and the prior parameters"
          << " do not refer to the transformed estimate for the @estimate "
          << estimate_label_ << ". This is not advised as it may cause bias errors. Please consult the User Manual.";
      }
      if (estimate->transform_with_jacobian_is_defined()) {
        if (transform_with_jacobian_ != estimate->transform_with_jacobian()) {
          LOG_ERROR_P(PARAM_LABEL) << "This parameter is not consistent with the equivalent parameter in the @estimate block "
            << estimate_label_ << ". Both parameters should be either true or false.";
        }
      }
      estimates_.push_back(estimate);
    }
  }


  // Validate that the parameters sum to one.
  Double total = 0.0;

  for (auto& estimate : estimates_) {
    LOG_FINEST() << "transformation value = " << estimate->value();
    total += estimate->value();
  }
  if (total != 1.0)
    LOG_ERROR_P(PARAM_ESTIMATE_LABELS) << "The parameter values do not sum to 1.0. They sum to " << total
      << ". Please check the initial values of these parameters.";

  // Check that the bounds are sensible
  if (parameters_.Get(PARAM_UPPER_BOUND)->has_been_defined() & parameters_.Get(PARAM_LOWER_BOUND)->has_been_defined()) {
    for (unsigned i = 0; i < estimates_.size(); ++i) {
      if (estimates_[i]->lower_bound() < 0.0 || estimates_[i]->lower_bound() > 1.0)
        LOG_ERROR_P(PARAM_LOWER_BOUND) << "The lower bound must be between 0.0 and 1.0 inclusive.";
      if (estimates_[i]->upper_bound() < 0.0 || estimates_[i]->upper_bound() > 1.0)
        LOG_ERROR_P(PARAM_UPPER_BOUND) << "The upper bound must be between 0.0 and 1.0 inclusive.";
    }
  }
  LOG_MEDIUM() << "total = " << total;

  // Turn off the last estimate
  LOG_FINE() << "Turning off parameter. This parameter will not be estimated and is a function of other parameters "
    << estimates_[estimates_.size() - 1]->parameter() << " in the estimation";
  estimates_[estimates_.size() - 1]->set_estimated(false);
  LOG_MEDIUM() << "flagged estimated = " << estimates_[estimates_.size() - 1]->estimated();
}

/**
 * Transform
 * This method will reset the lower and upper bounds
 */
void SumToOne::DoTransform() {
  LOG_TRACE();
  // reset the bounds for the others
  if (parameters_.Get(PARAM_UPPER_BOUND)->has_been_defined() && parameters_.Get(PARAM_LOWER_BOUND)->has_been_defined()) {
    for (unsigned i = 0; i < (estimates_.size() - 1); ++i) {
      estimates_[i]->set_lower_bound(lower_bounds_[i]);
      estimates_[i]->set_upper_bound(upper_bounds_[i]);
    }
  }

}
/**
 * Restore
 * This method will restore values provided by the minimiser that need to be restored for use in the annual cycle
 */
void SumToOne::DoRestore() {
  LOG_TRACE();
  // Create zk
  Double total = 0.0;
  for (unsigned i = 0; i < (estimates_.size() - 1); ++i) {
    total += estimates_[i]-> value();
  }
  Double new_value = 1.0 - total;
  LOG_FINE() << "Setting value to " << new_value << " for parameter = " << estimates_[estimates_.size() - 1]->parameter();
  estimates_[estimates_.size() - 1]->set_value(new_value);

}

/**
 * GetScore
 * Calculate the Jacobian, to offset the bias of the transformation that enters the objective function
 */
Double SumToOne::GetScore() {
  LOG_TRACE();
  jacobian_ = 0.0;
  LOG_MEDIUM() << "Jacobian: " << jacobian_;
  return jacobian_;
}

/**
 * Get the target addressables to ensure that each
 * object is not referencing multiple ones as this would
 * cause chain issues
 *
 * @return Set of addressable labels
 */
std::set<string> SumToOne::GetTargetEstimates() {
  set<string> result;
  for (auto& estimate_label : estimate_labels_) {
    result.insert(estimate_label);
  }
  return result;
}
} /* namespace estimatetransformations */
} /* namespace niwa */
