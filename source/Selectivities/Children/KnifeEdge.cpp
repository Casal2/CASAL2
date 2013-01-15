/**
 * @file KnifeEdge.cpp
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
#include "KnifeEdge.h"

// namespaces
namespace isam {
namespace selectivities {

/**
 * Default constructor (using constructor delegation)
 */
KnifeEdge::KnifeEdge()
: KnifeEdge(Model::Instance()) {
}

/**
 * Explicit Constructor
 */
KnifeEdge::KnifeEdge(ModelPtr model)
: Selectivity(model) {
  parameters_.RegisterAllowed(PARAM_ALPHA);
  parameters_.RegisterAllowed(PARAM_E);
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
void KnifeEdge::Validate() {
  LOG_TRACE();

  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_E);

  label_  = parameters_.Get(PARAM_LABEL).GetValue<string>();
  edge_   = parameters_.Get(PARAM_E).GetValue<double>();
  if (parameters_.IsDefined(PARAM_ALPHA))
    alpha_ = parameters_.Get(PARAM_ALPHA).GetValue<double>();
}

/**
 * Reset this selectivity so it's ready for the next execution
 * phase in the model.
 *
 * This method will rebuild the cache of selectivity values
 * for each age in the model.
 */
void KnifeEdge::Reset() {
  for (unsigned age = model_->min_age(); age <= model_->max_age(); ++age) {
    if (age >= edge_)
      values_[age] = alpha_;
    else
      values_[age] = 0.0;
  }
}

} /* namespace selectivities */
} /* namespace isam */
