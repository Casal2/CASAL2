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

#include <boost/math/distributions/lognormal.hpp>
#include <cmath>

#include "AgeLengths/AgeLength.h"
#include "Model/Model.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Explicit Constructor
 */
AllValues::AllValues(Model* model)
  : Selectivity(model) {

  parameters_.Bind<Double>(PARAM_V, &v_, "V", "");

  RegisterAsAddressable(PARAM_V, &v_);

  RegisterAsAddressable(PARAM_VALUES, &values_for_lookup_,addressable::kLookup);
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
void AllValues::DoValidate() {
  if (v_.size() != model_->age_spread()) {
    LOG_ERROR_P(PARAM_V) << ": Number of 'v' values supplied is not the same as the model age spread.\n"
        << "Expected: " << model_->age_spread() << " but got " << v_.size();
  }

  //initialise values container
  for (unsigned age = model_->min_age(); age <= model_->max_age(); ++age)
    values_for_lookup_[age] = 0.0;
}

/**
 * Reset this selectivity so it's ready for the next execution
 * phase in the model.
 *
 * This method will rebuild the cache of selectivity values
 * for each age in the model.
 */
void AllValues::Reset() {
  unsigned min_age = model_->min_age();
  for (unsigned i = 0; i < v_.size(); ++i) {
    values_[min_age + i] = v_[i];
    values_for_lookup_[min_age + i] = v_[i];
  }
}


/**
 * GetLengthBasedResult function
 *
 * @param age
 * @param age_length AgeLength pointer
 * @return Double selectivity for an age based on age length distribution
 */

Double AllValues::GetLengthBasedResult(unsigned age, AgeLength* age_length) {
  LOG_ERROR_P(PARAM_LENGTH_BASED) << ": This selectivity type has not been implemented for length based selectivities ";
  return 0.0;
}

} /* namespace selectivities */
} /* namespace niwa */
