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

#include <boost/math/distributions/normal.hpp>

#include "AgeLengths/AgeLength.h"
#include "Model/Model.h"
#include "TimeSteps/Manager.h"

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

  RegisterAsAddressable(PARAM_VALUES, &values_, addressable::kLookup);
  RegisterAsAddressable(PARAM_LENGTH_VALUES, &length_values_, addressable::kLookup);
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
  else {
    LOG_CODE_ERROR() << "Selectivity does not recognise the current partition_type. It's not length or age";
  }
  age_index_ = model_->min_age();

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

  if (model_->partition_type() == PartitionType::kAge) {
    values_.assign(model_->age_spread(), 0.0);
  } else {
    length_values_.assign(model_->length_bins().size(), 0.0);
  }
}


/**
 *
 */
void Selectivity::Reset() {
  if (is_estimated_) {
    RebuildCache();
  }
}

/**
 * Return the cached value for the specified age or length from
 * our internal map
 *
 * @param age_or_length The age or length to get selectivity value for
 * @param time_step_index The time step we want to use for length based results
 * @return The value stored in the map or 0.0 as default
 */

Double Selectivity::GetAgeResult(unsigned age, AgeLength* age_length) {
  if (!length_based_) {
    if (age - age_index_ >= values_.size())
      LOG_CODE_ERROR() << "if (age - age_index_ >= values_.size()) : age: " << age << "; label: " << label_ << "; type: " << type_;
    if (age < age_index_)
      LOG_CODE_ERROR() << "if (age < age_index)";
    return values_[age - age_index_];
  }

  return GetLengthBasedResult(age, age_length);
}

/**
 *
 */
Double Selectivity::GetLengthResult(unsigned length_bin_index) {
  return length_values_[length_bin_index];
}


/**
 * This method will return a pointer to a 4D vector object
 * containing age length cached values for the target age_length object.
 *
 * If the target object does not exist, then it will build it.
 */
Vector3* Selectivity::GetCache(AgeLength* age_length) {
  RebuildCache();
  // size varies for multi-dimensional vector
  unsigned year_count = model_->years().size();
  unsigned time_step_count = model_->managers().time_step()->size();
  unsigned age_count = model_->age_spread();

  if (!length_based_) {
    Vector3* new_vector = &cached_age_length_values_[PARAM_NONE];
    utilities::allocate_vector3(new_vector, year_count, time_step_count, age_count);

//    cout << "v: " << label_ << " | ";
//    for (unsigned i = 0; i < values_.size(); ++i)
//      cout << values_[i] << ", ";
//    cout << endl;

    for (unsigned i = 0; i < year_count; ++i)
      for (unsigned j = 0; j < time_step_count; ++j) {
        for (unsigned k = 0; k < age_count; ++k)
        (*new_vector)[i][j].assign(values_.begin(), values_.end());
      }

    return new_vector;

  }

  // This is age length code now.
  if (age_length == nullptr)
    LOG_CODE_ERROR() << "(age_length == nullptr)";

  if (cached_age_length_values_.find(age_length->label()) != cached_age_length_values_.end()) {
    // return existing cache
    return &cached_age_length_values_.find(age_length->label())->second;
  }

  // It does not exist, let's build it.
  Vector3* new_vector = &cached_age_length_values_[age_length->label()];
  utilities::allocate_vector3(new_vector, year_count, time_step_count, age_count);

  for (unsigned i = 0; i < year_count; ++i) {
    unsigned year = model_->start_year() + i;
    for (unsigned j = 0; j < time_step_count; ++j) {
      for (unsigned k = 0; k < age_count; ++k) {
        unsigned age = model_->min_age() + k;
        (*new_vector)[i][j][k] = this->GetLengthBasedResult(age, age_length, year, j);
      }
    }
  }

  return new_vector;
}

} /* namespace niwa */
