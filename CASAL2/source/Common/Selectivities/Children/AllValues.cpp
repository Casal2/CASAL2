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

#include "Age/AgeLengths/AgeLength.h"
#include "Common/Model/Model.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Explicit Constructor
 */
AllValues::AllValues(Model* model)
: Selectivity(model) {

  parameters_.Bind<Double>(PARAM_V, &v_, "V", "")->set_partition_type(PartitionType::kAge | PartitionType::kLength);

  RegisterAsAddressable(PARAM_V, &v_);
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
  switch(model_->partition_type()) {
  case PartitionType::kAge:
    if (v_.size() != model_->age_spread()) {
      LOG_ERROR_P(PARAM_V) << ": Number of 'v' values supplied is not the same as the model age spread.\n"
          << "Expected: " << model_->age_spread() << " but got " << v_.size();
    }
    break;

  case PartitionType::kLength:
    if (v_.size() != model_->length_bins().size()) {
      LOG_ERROR_P(PARAM_V) << ": Number of 'v' values supplied is not the same as the model length bin count.\n"
          << "Expected: " << model_->length_bins().size() << " but got " << v_.size();
    }
    break;

  default:
    LOG_CODE_ERROR() << "Unknown partition_type on the model at this point";
    break;
  }
}

/**
 * Reset this selectivity so it's ready for the next execution
 * phase in the model.
 *
 * This method will rebuild the cache of selectivity values
 * for each age in the model.
 */
void AllValues::RebuildCache() {
  if (model_->partition_type() == PartitionType::kAge) {
    unsigned min_age = model_->min_age();
    for (unsigned i = 0; i < v_.size(); ++i) {
      if (v_[i] < 0.0)
        LOG_FATAL_P(PARAM_V) << "cannot have value < 0.0 in this class. Found value = " << v_[i] << " for age = " << min_age + i;
      values_[i] = v_[i];
    }
  } else if (model_->partition_type() == PartitionType::kLength) {
    for (unsigned i = 0; i < v_.size(); ++i) {
      length_values_[i] = v_[i];
    }
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
  LOG_ERROR_P(PARAM_LENGTH_BASED) << ": This selectivity type has not been implemented for age length based selectivities ";
  return 0.0;
}

} /* namespace selectivities */
} /* namespace niwa */
