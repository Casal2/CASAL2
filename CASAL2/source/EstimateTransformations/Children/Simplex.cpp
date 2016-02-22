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

// namespaces
namespace niwa {
namespace estimatetransformations {
namespace utils = niwa::utilities;
/**
 * Default constructor
 */
Simplex::Simplex(Model* model) : EstimateTransformation(model) {
  parameters_.Bind<string>(PARAM_ESTIMATE, &estimate_label_, "Estimate to transform", "");
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
  estimates_ = model_->managers().estimate()->GetEstimatesByLabel(estimate_label_);
  if (estimates_.size() < 1) {
    LOG_ERROR_P(PARAM_ESTIMATE) << "Estimate " << estimate_label_ << " could not be found. Have you defined it?";
    return;
  }

  length_ = estimates_.size();
  simplex_values_.assign(length_, 0.0);

//  for (unsigned i = 0; i < length_ - 1; ++i) {
//    Estimate* estimate = model_->managers().estimate().creator().Create(PARAM_ESTIMATE, PARAM_UNIFORM);  // Giving the estimate a prior of uniform. i.e. no contribution
//    estimate->set_is_in_objective(false);
//    estimate->set_label("simplex_" + i);
//    estimate->set_target(&simplex_values[i]);
//    estimate->set_lower_bound(x);
//    estimate->set_upper_bound(y);
//    estimate->set_creator_parameter("simplex_" + i);
//  }
//
//  for (auto parameter : estimate_parameters) {
//    auto estimate = model_->managers().estimate()->GetEstimate(parameter);
//    estimate->set_estimated(false);
//    estimates_.push_back(estimate);
//  }
//

//
//  estimate_ = model_->managers().estimate()->GetEstimateByLabel(estimate_label_);
//  if (estimate_ == nullptr) {
//    LOG_ERROR_P(PARAM_ESTIMATE) << "Estimate " << estimate_label_ << " could not be found. Have you defined it?";
//    return;
//  }
//
//  // Find estimate create a new label for pseudo parameter of format type[label].simplex_parameter
//  string type       = "";
//  string label      = "";
//  string parameter  = "";
//  string index      = "";
//  model_->objects().ExplodeString(estimate_label_, type, label, parameter, index);
//  string new_parameter = type + "[" + label + "]." + "simplex_" + index;
//  LOG_FINEST() << "parameter: " << estimate_label_ << "; new_parameter: " << new_parameter;
//

}

/**
 *
 */
void Simplex::Transform() {
  // disable original estimate
  // enable pseudo estimate switching all switches neccassary for the transformation
  // set upper and lower bounds

}

/**
 *
 */
void Simplex::Restore() {
  // disable pseudo estimate and enable original parameter
  // change bounds back

}

/**
 * Get the target estimables so we can ensure each
 * object is not referencing multiple ones as this would
 * cause chain issues
 *
 * @return Set of estimable labels
 */
std::set<string> Simplex::GetTargetEstimates() {
  set<string> result;
  result.insert(estimate_label_);
  return result;
}
} /* namespace estimatetransformations */
} /* namespace niwa */
