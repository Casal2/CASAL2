/*
 * Estimate.cpp
 *
 *  Created on: 14/01/2013
 *      Author: Admin
 */

#include "Estimate.h"

namespace isam {

Estimate::Estimate() {
  // TODO Auto-generated constructor stub

}

Estimate::~Estimate() {
  // TODO Auto-generated destructor stub
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

}

} /* namespace isam */
