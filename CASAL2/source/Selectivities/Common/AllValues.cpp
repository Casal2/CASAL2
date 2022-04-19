/**
 * @file AllValues.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 14/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "AllValues.h"

#include <boost/math/distributions/lognormal.hpp>
#include <cmath>

#include "../../AgeLengths/AgeLength.h"
#include "../../Model/Model.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Default Constructor
 */
AllValues::AllValues(shared_ptr<Model> model) : Selectivity(model) {
  parameters_.Bind<Double>(PARAM_V, &v_, "The v parameter", "")->set_partition_type(PartitionType::kAge | PartitionType::kLength);

  RegisterAsAddressable(PARAM_V, &v_);
  allowed_length_based_in_age_based_model_ = false;

}

/**
 * Validate this selectivity. This will load the
 * values that were passed in from the configuration
 * file and assign them to the local variables.
 *
 * Then do some basic checks on the local
 * variables to ensure they are within the business
 * rules for the model.
 */
void AllValues::DoValidate() {
  switch (model_->partition_type()) {
    case PartitionType::kAge:
      if (v_.size() != model_->age_spread()) {
        LOG_ERROR_P(PARAM_V) << ": Number of 'v' values supplied is not the same as the number of ages classes that were defined for the model.\n"
                             << "Expected: " << model_->age_spread() << ", parsed: " << v_.size();
      }
      break;

    case PartitionType::kLength:
      if (v_.size() != model_->length_bin_mid_points().size()) {
        LOG_ERROR_P(PARAM_V) << ": Number of 'v' values supplied is not the same as the number of length bins that were defined for the model.\n"
                             << "Expected: " << model_->length_bin_mid_points().size() << ", parsed: " << v_.size();
      }
      break;

    default:
      LOG_CODE_ERROR() << "Unknown partition_type";
      break;
  }
}


/**
 * The core function
 */
Double AllValues::get_value(Double value) {
  if (model_->partition_type() == PartitionType::kLength) {
    return v_[model_->get_length_bin_ndx(value)];
  }
  LOG_CODE_ERROR() << "AllValues::get_value(Double value) value = "  << value;
  return 1.0;
}

/**
 * The core function
 */
Double AllValues::get_value(unsigned value) {
  if (model_->partition_type() == PartitionType::kAge) {
    return v_[value - model_->min_age()];
  } else {

  }
  LOG_CODE_ERROR() << "AllValues::get_value(unsigned value) value = "  << value;
  return 1.0;
}

} /* namespace selectivities */
} /* namespace niwa */
