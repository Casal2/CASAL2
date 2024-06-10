/**
 * @file Selectivity.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 11/01/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Selectivity.h"

#include <boost/math/distributions/normal.hpp>
#include <boost/math/distributions/lognormal.hpp>
#include <cmath>

#include "../AgeLengths/AgeLength.h"
#include "../Model/Model.h"
#include "../TimeSteps/Manager.h"

// Namesapces
namespace niwa {

/**
 * Default Constructor
 */
Selectivity::Selectivity(shared_ptr<Model> model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "The label for the selectivity", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "The type of selectivity", "");
  parameters_.Bind<bool>(PARAM_LENGTH_BASED, &length_based_, "Is the selectivity length based?", "", false);
  parameters_.Bind<unsigned>(PARAM_INTERVALS, &n_quant_, "The number of quantiles to evaluate a length-based selectivity over the age-length distribution", "", 5);
  // parameters_.Bind<string>(PARAM_PARTITION_TYPE, &partition_type_label_, "The type of partition that this selectivity will support. Defaults to the same as the model", "",
  // PARAM_MODEL)->set_allowed_values({PARAM_MODEL, PARAM_AGE, PARAM_LENGTH, PARAM_HYBRID});
  RegisterAsAddressable(PARAM_VALUES, &values_, addressable::kLookup);
  RegisterAsAddressable(PARAM_LENGTH_VALUES, &length_values_, addressable::kLookup);

  // Default to type model. Remove this line once additional partition types are implemented
  partition_type_label_ = PARAM_MODEL;
}

/**
 * Validate the objects
 */
void Selectivity::Validate() {
  parameters_.Populate(model_);
  LOG_FINE() << "selectivity " << label_ << " type = " << type_;

  if (partition_type_label_ == PARAM_MODEL)
    partition_type_ = model_->partition_type();
  else if (partition_type_label_ == PARAM_AGE)
    partition_type_ = PartitionType::kAge;
  else if (partition_type_label_ == PARAM_LENGTH)
    partition_type_ = PartitionType::kLength;
  else {
    LOG_CODE_ERROR() << "The partition_type is not recognized for this selectivity. It is not availabe for a length or age";
  }
  age_index_ = model_->min_age();

  DoValidate();

  if (length_based_) {
    boost::math::normal dist{};

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
    length_values_.assign(model_->length_bin_mid_points().size(), 0.0);
  }
  if(length_based_) {
    if(!allowed_length_based_in_age_based_model_)
      LOG_ERROR_P(PARAM_LENGTH_BASED) << "this selectivity type is not allowed to be length based.";
  }
  
}

/**
 * Reset the objects
 */
void Selectivity::Reset() {
  LOG_FINE() << "Selectivity::Reset() " << label_;
  DoReset();
  RebuildCache();
}

/**
 * Virtual method for children to implement
 * overriden if child has it specified
 */
void Selectivity::DoReset() {

}
/**
 * Reset the objects
 */
void Selectivity::RebuildCache() {
  LOG_FINE() << "RebuildCache label " << label_;
  if (model_->partition_type() == PartitionType::kAge) {
    for (unsigned age = model_->min_age(); age <= model_->max_age(); ++age) {
      values_[age - age_index_] = this->get_value(age);
      LOG_FINEST( ) << "age = " << age << " value = " << values_[age - age_index_] ;
    }
  } else if (model_->partition_type() == PartitionType::kLength) {
    vector<double> length_bins = model_->length_bin_mid_points();
    Double         temp        = 0.0;
    for (unsigned length_bin_index = 0; length_bin_index < length_bins.size(); ++length_bin_index) {
      temp = length_bins[length_bin_index];
      length_values_[length_bin_index] = this->get_value(temp);
      LOG_FINEST( ) << "length = " << temp << " value = " << length_values_[length_bin_index];
    }
  }
}

/**
 * Return the cached value for the specified age or length from
 * the internal map
 *
 * @param age The age  to get selectivity value for
 * @param age_length pointer to age-length object
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
 * Return the length values for a length bin
 *
 * @param length_bin_index The index of the length bin
 * @return the length value
 */
Double Selectivity::GetLengthResult(unsigned length_bin_index) {
  return length_values_[length_bin_index];
}
/**
 * Return the selectivity values for the value x
 * currently used in AgeLength::get_pdf_with_sized_based_selectivity
 * But will be useful else where. Not advised to be used for other methods because the other
 * methods cache and do a look up. This method will ask the selectivity to always calculate the result even if nothin
 * has changed.
 * @param x The value to evaluate the selectivity for
 * @return the selectivity by x value
 */
Double Selectivity::GetResult(Double x) {
  return get_value(x);
}
/**
 * Return the length values for a length bin
 *
 * @param age The index of the length bin
 * @param age_length pointer
 * @return age-based value based on integrating over the age-length distribution
 */
Double Selectivity::GetLengthBasedResult(unsigned age, AgeLength* age_length) {
  LOG_TRACE();
  unsigned time_step = model_->managers()->time_step()->current_time_step();
  unsigned year = model_->current_year();
  Double   cv        = age_length->cv(year, time_step, age);
  LOG_FINE() << "times step " << time_step << " year = " << year;
  Double   mean      = age_length->mean_length(time_step, age);
  string   dist      = age_length->distribution_label();
  LOG_FINEST() << "mu = " << mean << " cv = " << cv <<  " dist = " << dist << " n quant " << n_quant_;

  if (dist == PARAM_NONE || n_quant_ <= 1) {
    LOG_FINEST() << "value = " << this->get_value(mean);
    return this->get_value(mean);
  } else if (dist == PARAM_NORMAL) {
    Double sigma = cv * mean;
    Double size  = 0.0;
    Double total = 0.0;
    LOG_FINEST() << "mu = " << mean << " sigma = " << sigma << " n quant " << n_quant_;
    for (unsigned j = 0; j < n_quant_; ++j) {
      size = mean + sigma * quantiles_at_[j];
      total += this->get_value(size);
      LOG_FINEST() << "size = " << size << " value = " << this->get_value(size) << " total = " << total;
    }
    return total / n_quant_;

  } else if (dist == PARAM_LOGNORMAL) {
    // convert paramters to log space
    Double                 sigma = sqrt(log(1 + cv * cv));
    Double                 mu    = log(mean) - sigma * sigma * 0.5;
    Double                 size  = 0.0;
    Double                 total = 0.0;
    LOG_FINEST() << "mu = " << mu << " sigma = " << sigma;
    boost::math::lognormal dist{AS_DOUBLE(mu), AS_DOUBLE(sigma)};
    for (unsigned j = 0; j < n_quant_; ++j) {
      size = quantile(dist, AS_DOUBLE(quantiles_[j]));
      total += this->get_value(size);
      LOG_FINEST() << "size = " << size << " value = " << this->get_value(size) << " total = " << total;
    }
    return total / n_quant_;
  }
  LOG_CODE_ERROR() << "The specified distribution is not a valid distribution: " << dist;
  return 0;
}


/**
 * This method returns a pointer to a 4D vector object
 * containing age length cached values for the target age_length object.
 *
 * If the target object does not exist, then it will build it.
 */
Vector3* Selectivity::GetCache(AgeLength* age_length) {
  RebuildCache();
  // size varies for multi-dimensional vector
  unsigned year_count      = model_->years().size();
  unsigned time_step_count = model_->managers()->time_step()->size();
  unsigned age_count       = model_->age_spread();

  if (!length_based_) {
    Vector3* new_vector = &cached_age_length_values_[PARAM_NONE];
    utilities::allocate_vector3(new_vector, year_count, time_step_count, age_count);

    //    cout << "v: " << label_ << " | ";
    //    for (unsigned i = 0; i < values_.size(); ++i)
    //      cout << values_[i] << ", ";
    //    cout << endl;

    for (unsigned i = 0; i < year_count; ++i)
      for (unsigned j = 0; j < time_step_count; ++j) {
        for (unsigned k = 0; k < age_count; ++k) (*new_vector)[i][j].assign(values_.begin(), values_.end());
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
    for (unsigned j = 0; j < time_step_count; ++j) {
      for (unsigned k = 0; k < age_count; ++k) {
        unsigned age           = model_->min_age() + k;
        (*new_vector)[i][j][k] = this->GetLengthBasedResult(age, age_length);
      }
    }
  }

  return new_vector;
}

} /* namespace niwa */
