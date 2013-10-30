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
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label");
  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "The name of the variable to estimate in the model");
  parameters_.Bind<double>(PARAM_LOWER_BOUND, &lower_bound_, "The lowest value the parameter is allowed to have");
  parameters_.Bind<double>(PARAM_UPPER_BOUND, &upper_bound_, "The highest value the parameter is allowed to have");
  parameters_.Bind<string>(PARAM_PRIOR, &prior_label_, "The name of the prior to use for the parameter", "");
  parameters_.Bind<string>(PARAM_SAME, &same_labels, "A list of parameters that are bound to the value of this estimate", "");
  parameters_.Bind<unsigned>(PARAM_ESTIMATION_PHASE, &estimation_phase_, "TBA", 1u);
  parameters_.Bind<bool>(PARAM_MCMC, &mcmc_fixed_, "TBA", "");
}

/**
 * Validate our estimate. Some of the
 * validation was done by the
 * estimates::Info object before the
 * estimate was created so we can skip that.
 */
void Estimate::Validate() {
  parameters_.Populate();

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
