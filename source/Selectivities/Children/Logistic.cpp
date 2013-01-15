/**
 * @file Logistic.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Logistic.h"

#include <cmath>

#include "Model/Model.h"

// Namespaces
namespace isam {
namespace selectivities {

/**
 * Default Constructor
 */
Logistic::Logistic()
: Selectivity(Model::Instance()) {
  parameters_.RegisterAllowed(PARAM_A50);
  parameters_.RegisterAllowed(PARAM_ATO95);
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
void Logistic::Validate() {
  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_A50);
  CheckForRequiredParameter(PARAM_ATO95);

  label_  = parameters_.Get(PARAM_LABEL).GetValue<string>();
  a50_    = parameters_.Get(PARAM_A50).GetValue<double>();
  aTo95_  = parameters_.Get(PARAM_ATO95).GetValue<double>();

  if (parameters_.IsDefined(PARAM_ALPHA))
    alpha_ = parameters_.Get(PARAM_ALPHA).GetValue<double>();

  if (alpha_ <= 0.0)
    LOG_ERROR(parameters_.location(PARAM_ALPHA) << ": alpha (" << alpha_ << ") cannot be less than or equal to 0.0");
  if (aTo95_ <= 0.0)
    LOG_ERROR(parameters_.location(PARAM_ATO95) << ": ato95 (" << aTo95_ << ") cannot be less than or equal to 0.0");
}

/**
 * Reset this selectivity so it's ready for the next execution
 * phase in the model.
 *
 * This method will rebuild the cache of selectivity values
 * for each age in the model.
 */
void Logistic::Reset() {
  double threshold = 0.0;

  ModelPtr model = Model::Instance();
  for (unsigned age = model->min_age(); age <= model->max_age(); ++age) {
    threshold = (a50_ - age) / aTo95_;

    if (threshold > 5.0)
      values_[age] = 0.0;
    else if (threshold < -5.0)
      values_[age] = alpha_;
    else
      values_[age] = alpha_ / (1.0 + pow(19.0, threshold));
  }
}

} /* namespace selectivities */
} /* namespace isam */
