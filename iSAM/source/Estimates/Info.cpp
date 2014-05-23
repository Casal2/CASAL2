/**
 * @file Info.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 27/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Info.h"

#include "Estimates/Factory.h"
#include "ObjectsFinder/ObjectsFinder.h"

// Namespaces
namespace isam {
namespace estimates {

/**
 * Default Constructor
 */
Info::Info() {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type", "");
  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "The name of the variable to estimate in the model", "", "");
  parameters_.Bind<double>(PARAM_LOWER_BOUND, &lower_bound_, "The lowest value the parameter is allowed to have", "");
  parameters_.Bind<double>(PARAM_UPPER_BOUND, &upper_bound_, "The highest value the parameter is allowed to have", "");
  parameters_.Bind<string>(PARAM_PRIOR, &prior_label_, "The name of the prior to use for the parameter", "", "");
  parameters_.Bind<string>(PARAM_SAME, &same_, "A list of parameters that are bound to the value of this estimate", "", "");
  parameters_.Bind<string>(PARAM_ESTIMATION_PHASE, &estimation_phase_, "TBA", "", "");
  parameters_.Bind<string>(PARAM_MCMC, &mcmc_, "TBA", "", "");
}

/**
 * Destructor
 */
Info::~Info() {
}

/**
 * This method will validate the parameters passed to the info
 * object before we attempt to build any actual estimates
 */
void Info::Validate() {
  LOG_TRACE();

  parameters_.Populate();

  if (lower_bound_ >= upper_bound_) {
    LOG_ERROR(parameters_.location(PARAM_LOWER_BOUND) << ": lower_bound cannot be equal to or greater than upper_bound");
  }
}

/**
 * This method will take all of the values that were passed in
 * by the configuration file and build a collection of estimate
 * objects from it.
 */
void Info::BuildEstimates() {
  LOG_TRACE();

  string type       = "";
  string label      = "";
  string parameter  = "";
  string index      = "";

  type_ = utilities::ToLowercase(type_);

  /**
   * Explode the parameter string so we can get the estimable
   * name (parameter) and the index
   */
  if (parameter_ == "") {
    parameters().Add(PARAM_PARAMETER, label_, parameters_.Get(PARAM_LABEL)->file_name(), parameters_.Get(PARAM_LABEL)->line_number());
    parameter_ = label_;
  }

  objects::ExplodeString(parameter_, type, label, parameter, index);
  if (type == "" || label == "" || parameter == "") {
    LOG_ERROR(parameters_.location(PARAM_PARAMETER) << ": parameter " << parameter_
        << " is not in the correct format. Correct format is object_type[label].estimable(array index)");
  }

  /**
   * rebuild the parameter removing any case issues we may have
   */
  objects::ImplodeString(type, label, parameter, index, parameter_);
  parameters_.Get(PARAM_PARAMETER)->set_value(parameter_);

  base::ObjectPtr target = objects::FindObject(parameter_);
  if (!target) {
    LOG_ERROR(parameters_.location(PARAM_PARAMETER) << ": parameter " << parameter_ << " is not a valid estimable in the system");
  }

  if (index == "" || !target->IsEstimableAVector(parameter)) {
    // Create one estimate
    isam::EstimatePtr estimate = isam::estimates::Factory::Create(PARAM_ESTIMATE, type_);
    if (!estimate)
      LOG_CODE_ERROR("Failed to create estimate with type: " << type_);
    estimate->parameters().CopyFrom(parameters_);

    if (index != "")
      parameter += "(" + index + ")";
    estimate->set_target(target->GetEstimable(parameter));
    estimate->set_parent_info(shared_from_this());

  } else {
    // Create N estimates
    unsigned n = target->GetEstimableSize(parameter);
    for (unsigned i = 0; i < n; ++i) {
      isam::EstimatePtr estimate = isam::estimates::Factory::Create(PARAM_ESTIMATE, type_);
      if (!estimate)
        LOG_CODE_ERROR("Failed to create estimate with type: " << type_);

      string new_parameter = parameter + "(" + utilities::ToInline<unsigned, string>(i+1) + ")";
      estimate->parameters().CopyFrom(parameters_);
      estimate->set_target(target->GetEstimable(new_parameter));
      estimate->set_parent_info(shared_from_this());
    }
  }
}

} /* namespace estimates */
} /* namespace isam */
