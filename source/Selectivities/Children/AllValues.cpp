/**
 * @file AllValues.cpp
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
#include "AllValues.h"
#include "Model/Model.h"

// Namespaces
namespace isam {
namespace selectivities {

/**
 * Default constructor
 */
AllValues::AllValues()
: Selectivity(Model::Instance()) {
  parameters_.RegisterAllowed(PARAM_V);
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
void AllValues::Validate() {
  LOG_TRACE();

  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_V);

  // Vs should be the same length as the world age spread
  label_  = parameters_.Get(PARAM_LABEL).GetValue<string>();
  v_      = parameters_.Get(PARAM_V).GetValues<double>();

  ModelPtr model = Model::Instance();
  if (v_.size() != model->age_spread()) {
    LOG_ERROR(parameters_.location(PARAM_V) << ": Number of 'v' values supplied is not the same as the model age spread.\n"
        << "Expected: " << model->age_spread() << " but got " << v_.size());
  }

  // TODO: Register v_ as estimable
}

/**
 * Reset this selectivity so it's ready for the next execution
 * phase in the model.
 *
 * This method will rebuild the cache of selectivity values
 * for each age in the model.
 */
void AllValues::Reset() {
  ModelPtr model = Model::Instance();

  unsigned min_age = model->min_age();
  for (unsigned i = 0; i < v_.size(); ++i) {
    values_[min_age + i] = v_[i];
  }
}

} /* namespace selectivities */
} /* namespace isam */
