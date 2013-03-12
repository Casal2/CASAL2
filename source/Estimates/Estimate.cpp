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

#include "Priors/Manager.h"

// Namespaces
namespace isam {

/**
 * Default constructor
 */
Estimate::Estimate() {
}

/**
 * Destructor
 */
Estimate::~Estimate() {
}

/**
 * Validate our estimate. Some of the
 * validation was done by the
 * estimates::Info object before the
 * estimate was created so we can skip that.
 */
void Estimate::Validate() {
  CheckForRequiredParameter(PARAM_LABEL); // Checking to make sure we didn't code an error
  CheckForRequiredParameter(PARAM_PARAMETER);

  label_            = parameters_.Get(PARAM_LABEL).GetValue<string>();
  parameter_        = parameters_.Get(PARAM_PARAMETER).GetValue<string>();
  lower_bound_      = parameters_.Get(PARAM_LOWER_BOUND).GetValue<double>();
  upper_bound_      = parameters_.Get(PARAM_UPPER_BOUND).GetValue<double>();
  mcmc_fixed_       = parameters_.Get(PARAM_MCMC_FIXED).GetValue<bool>(false);
  prior_label_      = parameters_.Get(PARAM_PRIOR).GetValue<string>("");
  estimation_phase_ = parameters_.Get(PARAM_ESTIMATION_PHASE).GetValue<unsigned>(1);
  same_labels       = parameters_.Get(PARAM_SAME).GetValues<string>();

  map<string, unsigned> same_count;
  for(string same : same_labels) {
    same_count[same]++;
    if (same_count[same] > 1) {
      LOG_ERROR(parameters_.location(PARAM_SAME) << ": same parameter '" << same << "' is a duplicate. Please remove this from your configuration file");
    }
  }
}

/**
 * Build our estimate. This is where we'll build the runtime
 * relationships to other objects in iSAM like the priors
 * and sames
 */
void Estimate::Build() {
  if (prior_label_ != "") {
    prior_ = priors::Manager::Instance().GetPrior(prior_label_);
    if (!prior_) {
      LOG_ERROR(parameters_.location(PARAM_PRIOR) << ": prior " << prior_label_ << " has not been specified as a @prior object in the configuration file");
    }
  }
}

/**
 * Caclulate and return the value of our prior
 *
 * @return Prior score
 */
Double Estimate::GetPriorScore() {
  if (!prior_) {
    LOG_CODE_ERROR("Prior == 0. No prior has been specified or built for this object");
  }

  return prior_->GetScore(*target_);
}

} /* namespace isam */
