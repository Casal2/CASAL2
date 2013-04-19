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
  parameters_.RegisterAllowed(PARAM_LABEL);
  parameters_.RegisterAllowed(PARAM_PARAMETER);
  parameters_.RegisterAllowed(PARAM_LOWER_BOUND);
  parameters_.RegisterAllowed(PARAM_UPPER_BOUND);
  parameters_.RegisterAllowed(PARAM_PRIOR);
  parameters_.RegisterAllowed(PARAM_SAME);
  parameters_.RegisterAllowed(PARAM_ESTIMATION_PHASE);
  parameters_.RegisterAllowed(PARAM_MCMC);
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
  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_PARAMETER);
  CheckForRequiredParameter(PARAM_LOWER_BOUND);
  CheckForRequiredParameter(PARAM_UPPER_BOUND);

  label_      = parameters_.Get(PARAM_LABEL).GetValue<string>();
  parameter_  = parameters_.Get(PARAM_PARAMETER).GetValue<string>();

  double lower_bound = parameters_.Get(PARAM_LOWER_BOUND).GetValue<double>();
  double upper_bound = parameters_.Get(PARAM_UPPER_BOUND).GetValue<double>();
  if (lower_bound >= upper_bound) {
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

  /**
   * Explode the parameter string so we can get the estimable
   * name (parameter) and the index
   */
  objects::ExplodeString(parameter_, type, label, parameter, index);
  if (type == "" || label == "" || parameter == "") {
    LOG_ERROR(parameters_.location(PARAM_PARAMETER) << ": parameter " << parameter_
        << " is not in the correct format. Correct format is object_type[label].estimable[array index]");
  }

  base::ObjectPtr target = objects::FindObject(parameter_);
  if (!target) {
    LOG_ERROR(parameters_.location(PARAM_PARAMETER) << ": parameter " << parameter_ << " is not a valid estimable in the system");
  }

  if (index == "" || !target->IsEstimableAVector(parameter)) {
    // Create one estimate
    isam::EstimatePtr estimate = isam::estimates::Factory::Create();
    estimate->parameters().CopyFrom(parameters_);

    if (index != "")
      parameter += "(" + index + ")";
    estimate->set_target(target->GetEstimable(parameter));

  } else {
    // Create N estimates
    unsigned n = target->GetEstimableSize(parameter);
    for (unsigned i = 0; i < n; ++i) {
      isam::EstimatePtr estimate = isam::estimates::Factory::Create();

      string new_parameter = parameter + "(" + utilities::ToInline<unsigned, string>(i+1) + ")";
      estimate->parameters().CopyFrom(parameters_);
      estimate->set_target(target->GetEstimable(new_parameter));
    }
  }
}

} /* namespace estimates */
} /* namespace isam */
