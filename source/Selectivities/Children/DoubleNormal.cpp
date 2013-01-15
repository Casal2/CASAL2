/**
 * @file DoubleNormal.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 14/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "DoubleNormal.h"

#include <cmath>

#include "Model/Model.h"

// Namespaces
namespace isam {
namespace selectivities {

/**
 * Constructor
 */
DoubleNormal::DoubleNormal()
: Selectivity(Model::Instance()) {
  parameters_.RegisterAllowed(PARAM_MU);
  parameters_.RegisterAllowed(PARAM_SIGMA_L);
  parameters_.RegisterAllowed(PARAM_SIGMA_R);
  parameters_.RegisterAllowed(PARAM_ALPHA);
}

/**
 * Validate this selectivity. This will load the
 * values that were passed in from the configuration
 * file and assign them to the local variables.
 *
 * We'll then do some basic checks on the local
 * variables to ensure they are within the business
 * rules for the model.
 */
void DoubleNormal::Validate() {
  LOG_TRACE();

  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_MU);
  CheckForRequiredParameter(PARAM_SIGMA_L);
  CheckForRequiredParameter(PARAM_SIGMA_R);
  CheckForRequiredParameter(PARAM_ALPHA);

  label_    = parameters_.Get(PARAM_LABEL).GetValue<string>();
  mu_       = parameters_.Get(PARAM_MU).GetValue<double>();
  sigma_l_  = parameters_.Get(PARAM_SIGMA_L).GetValue<double>();
  sigma_r_  = parameters_.Get(PARAM_SIGMA_R).GetValue<double>();
  alpha_    = parameters_.Get(PARAM_ALPHA).GetValue<double>();

  if (alpha_ <= 0.0)
    LOG_ERROR(parameters_.location(PARAM_MU) << ": alpha cannot be less than or equal to 0.0");
  if (sigma_l_ <= 0.0)
    LOG_ERROR(parameters_.location(PARAM_SIGMA_L) << ": sigma_l cannot be less than or equal to 0.0");
  if (sigma_r_ <= 0.0)
    LOG_ERROR(parameters_.location(PARAM_SIGMA_R) << ": sigmal_r cannot be less than or equal to 0.0");
}

/**
 * Reset this selectivity so it's ready for the next execution
 * phase in the model.
 *
 * This method will rebuild the cache of selectivity values
 * for each age in the model.
 */
void DoubleNormal::Reset() {
  ModelPtr model = Model::Instance();
  for (unsigned age = model->min_age(); age <= model->max_age(); ++age) {
    if (age < mu_)
      values_[age] = pow(2.0, -((age - mu_) / sigma_l_ * (age - mu_) / sigma_l_)) * alpha_;
    else
      values_[age] = pow(2.0, -((age - mu_)/sigma_r_ * (age - mu_) / sigma_r_)) * alpha_;
  }
}

} /* namespace selectivities */
} /* namespace isam */
