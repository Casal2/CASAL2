/**
 * @file Selectivity.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 11/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Selectivity.h"

#include "Common/Model/Model.h"
#include "Age/AgeLengths/AgeLength.h"

#include <boost/math/distributions/normal.hpp>

// Namesapces
namespace niwa {

/**
 * Explicit Constructor
 */
Selectivity::Selectivity(Model* model)
: model_(model) {

  parameters_.Bind<string>(PARAM_LABEL, &label_, "The label for this selectivity", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "The type of selectivity", "");
  parameters_.Bind<bool>(PARAM_LENGTH_BASED, &length_based_, "Is the selectivity length based", "", false);
  parameters_.Bind<unsigned>(PARAM_INTERVALS, &n_quant_, "Number of quantiles to evaluate a length based selectivity over the age length distribution", "", 5);
  parameters_.Bind<string>(PARAM_PARTITION_TYPE, &partition_type_label_, "The type of partition this selectivity will support, Defaults to same as the model", "", PARAM_MODEL)
      ->set_allowed_values({PARAM_MODEL, PARAM_AGE, PARAM_LENGTH, PARAM_HYBRID});
}

/**
 *
 */
void Selectivity::Validate() {
  parameters_.Populate(model_);

  if (partition_type_label_ == PARAM_MODEL)
    partition_type_ = model_->partition_type();
  else if (partition_type_label_ == PARAM_AGE)
    partition_type_ = PartitionType::kAge;
  else if (partition_type_label_ == PARAM_LENGTH)
    partition_type_ = PartitionType::kLength;
//  else
//    partition_type_ = PartitionType::kHybrid;

  DoValidate();

  if (length_based_) {
    boost::math::normal dist{ };

    for (unsigned i = 1; i <= n_quant_; ++i) {
      quantiles_.push_back((Double(i) - 0.5) / Double(n_quant_));
      LOG_FINEST() << ": Quantile value = " << quantiles_[i - 1];
      quantiles_at_.push_back(quantile(dist, AS_DOUBLE(quantiles_[i - 1])));
      LOG_FINEST() << ": Normal quantile value = " << quantiles_at_[i - 1];
    }
  }
}


/**
 *
 */
void Selectivity::Reset() {
  values_.clear();
}

/**
 * Return the cached value for the specified age or length from
 * our internal map
 *
 * @param age_or_length The age or length to get selectivity value for
 * @return The value stored in the map or 0.0 as default
 */

Double Selectivity::GetAgeResult(unsigned age, AgeLength* age_length) {
  if (!length_based_)
    return values_[age];

  return GetLengthBasedResult(age, age_length);
}

/**
 *
 */
Double Selectivity::GetLengthResult(unsigned length_bin_index) {
  return values_[length_bin_index];
}



} /* namespace niwa */
