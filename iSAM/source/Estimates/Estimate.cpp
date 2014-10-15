/**
 * @file Estimate.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 8/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Estimate.h"

#include "Estimates/Transformations/Factory.h"

// Namespaces
namespace isam {

/**
 * Default constructor
 */
Estimate::Estimate() {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type", "");
  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "The name of the variable to estimate in the model", "", "");
  parameters_.Bind<double>(PARAM_LOWER_BOUND, &lower_bound_, "The lowest value the parameter is allowed to have", "");
  parameters_.Bind<double>(PARAM_UPPER_BOUND, &upper_bound_, "The highest value the parameter is allowed to have", "");
  parameters_.Bind<string>(PARAM_PRIOR, &prior_label_, "The name of the prior to use for the parameter", "", "");
  parameters_.Bind<string>(PARAM_SAME, &same_labels, "A list of parameters that are bound to the value of this estimate", "", "");
  parameters_.Bind<unsigned>(PARAM_ESTIMATION_PHASE, &estimation_phase_, "TBA", "", 1u);
  parameters_.Bind<bool>(PARAM_MCMC, &mcmc_fixed_, "TBA", "", "");
  parameters_.Bind<string>(PARAM_TRANSFORMATION, &transformation_details_, "", "", "");
}

/**
 * Validate our estimate. Some of the
 * validation was done by the
 * estimates::Info object before the
 * estimate was created so we can skip that.
 */
void Estimate::Validate() {
  parameters_.Populate();
  if (parameter_ == "")
    parameter_ = label_;

  map<string, unsigned> same_count;
  for(string same : same_labels) {
    same_count[same]++;
    if (same_count[same] > 1) {
      LOG_ERROR(parameters_.location(PARAM_SAME) << ": same parameter '" << same << "' is a duplicate. Please remove this from your configuration file");
    }
  }

  if (transformation_details_.size() != 0) {
    string type = transformation_details_[0];
    transformation_ = estimates::transformations::Factory::Create(type);
    if (transformation_details_.size() > 1) {
      vector<string> extra_parameters;
      extra_parameters.assign(transformation_details_.begin() + 1, transformation_details_.end());
    }
  }
  DoValidate();
}

/**
 * Return the value applying any transformation we may have
 */
Double Estimate::GetTransformedValue() {
  if (transformation_)
    return transformation_->Transform(*target_);

  return *target_;
}

void Estimate::SetTransformedValue(Double minimiser_value) {
  if (transformation_)
    *target_ = transformation_->Untransform(minimiser_value);
  else
    *target_ = minimiser_value;
}

} /* namespace isam */
