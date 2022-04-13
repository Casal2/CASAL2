/**
 * @file DoubleNormalPlateau.cpp
 * @author  C.Marsh
 * @version 1.0
 * @date 2020
 * @section LICENSE
 *
 * Copyright NIWA Science 2020 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "DoubleNormalPlateau.h"

#include <boost/math/distributions/lognormal.hpp>
#include <cmath>

#include "../../AgeLengths/AgeLength.h"
#include "../../Model/Model.h"
#include "../../TimeSteps/Manager.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Default constructor
 */
DoubleNormalPlateau::DoubleNormalPlateau(shared_ptr<Model> model) : Selectivity(model) {
  parameters_.Bind<Double>(PARAM_SIGMA_L, &sigma_l_, "The sigma L parameter", "")->set_lower_bound(0.0, false);
  parameters_.Bind<Double>(PARAM_SIGMA_R, &sigma_r_, "The sigma R parameter", "")->set_lower_bound(0.0, false);
  parameters_.Bind<Double>(PARAM_A1, &a1_, "The a1 parameter", "")->set_lower_bound(0.0, false);
  parameters_.Bind<Double>(PARAM_A2, &a2_, "The a2 parameter", "")->set_lower_bound(0.0, false);
  parameters_.Bind<Double>(PARAM_ALPHA, &alpha_, "The maximum value of the selectivity", "", 1.0)->set_lower_bound(0.0, false); // The equivalent of a_max



  RegisterAsAddressable(PARAM_A1, &a1_);
  RegisterAsAddressable(PARAM_A2, &a2_);
  RegisterAsAddressable(PARAM_SIGMA_L, &sigma_l_);
  RegisterAsAddressable(PARAM_SIGMA_R, &sigma_r_);
  RegisterAsAddressable(PARAM_ALPHA, &alpha_);
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
void DoubleNormalPlateau::DoValidate() {
  if (alpha_ <= 0.0)
    LOG_ERROR_P(PARAM_ALPHA) << ": alpha cannot be less than or equal to 0.0";
  if (sigma_l_ <= 0.0)
    LOG_ERROR_P(PARAM_SIGMA_L) << ": sigma_l cannot be less than or equal to 0.0";
  if (sigma_r_ <= 0.0)
    LOG_ERROR_P(PARAM_SIGMA_R) << ": sigmal_r cannot be less than or equal to 0.0";
}

/**
 * Reset this selectivity so it is ready for the next execution
 * phase in the model.
 *
 * This method will rebuild the cache of selectivity values
 * for each age or length in the model.
 */
void DoubleNormalPlateau::RebuildCache() {
  if (model_->partition_type() == PartitionType::kAge) {
    Double temp = 0.0;
    for (unsigned age = model_->min_age(); age <= model_->max_age(); ++age) {
      temp = age;
      if (temp < a1_)
        values_[age - age_index_] = alpha_ * pow(2.0,-((temp - a1_) / sigma_l_ * (temp-a1_)/sigma_l_));
      else if (temp < (a1_ + a2_)) 
        values_[age - age_index_] = alpha_;
      else 
        values_[age - age_index_] = alpha_*pow(2.0,-((temp-(a1_ + a2_))/sigma_r_ * (temp-(a1_ + a2_))/sigma_r_));
    }
  } else if (model_->partition_type() == PartitionType::kLength) {
    vector<double> length_bins = model_->length_bin_mid_points();
    Double         temp        = 0.0;
    for (unsigned length_bin_index = 0; length_bin_index < length_bins.size(); ++length_bin_index) {
      temp = length_bins[length_bin_index];
      if (temp < a1_)
        length_values_[length_bin_index]  = alpha_ * pow(2.0,-((temp - a1_) / sigma_l_ * (temp-a1_)/sigma_l_));
      else if (temp < (a1_ + a2_)) 
        length_values_[length_bin_index]  = alpha_;
      else 
        length_values_[length_bin_index]  = alpha_*pow(2.0,-((temp-(a1_ + a2_))/sigma_r_ * (temp-(a1_ + a2_))/sigma_r_));
    }
  }
}

/**
 * GetLengthBasedResult function
 *
 * @param age
 * @param age_length AgeLength pointer
 * @param year
 * @param time_step_index
 * @return Double selectivity for an age based on age length distribution_label
 */

Double DoubleNormalPlateau::GetLengthBasedResult(unsigned age, AgeLength* age_length, unsigned year, int time_step_index) {
  LOG_TRACE();
  unsigned yearx     = year == 0 ? model_->current_year() : year;
  unsigned time_step = time_step_index == -1 ? model_->managers()->time_step()->current_time_step() : (unsigned)time_step_index;
  Double   cv        = age_length->cv(yearx, time_step, age);
  Double   mean      = age_length->mean_length(time_step, age);
  string   dist      = age_length->distribution_label();

  if (dist == PARAM_NONE || n_quant_ <= 1) {
      if (mean < a1_)
        return alpha_ * pow(2.0,-((mean - a1_) / sigma_l_ * (mean-a1_)/sigma_l_));
      else if (mean < (a1_ + a2_)) 
        return  alpha_;
      else 
        return alpha_*pow(2.0,-((mean-(a1_ + a2_))/sigma_r_ * (mean-(a1_ + a2_))/sigma_r_));

  } else if (dist == PARAM_NORMAL) {
    Double sigma = cv * mean;
    Double size  = 0.0;
    Double total = 0.0;

    for (unsigned j = 0; j < n_quant_; ++j) {
      size = mean + sigma * quantiles_at_[j];
      if (size < a1_)
        total += alpha_ * pow(2.0,-((size - a1_) / sigma_l_ * (size-a1_)/sigma_l_));
      else if (mean < (a1_ + a2_)) 
        total +=  alpha_;
      else 
        total += alpha_*pow(2.0,-((size-(a1_ + a2_))/sigma_r_ * (size-(a1_ + a2_))/sigma_r_));
    }
    return total / n_quant_;

  } else if (dist == PARAM_LOGNORMAL) {
    // convert paramters to log space
    Double                 sigma = sqrt(log(1 + cv * cv));
    Double                 mu    = log(mean) - sigma * sigma * 0.5;
    Double                 size  = 0.0;
    Double                 total = 0.0;
    boost::math::lognormal dist{AS_DOUBLE(mu), AS_DOUBLE(sigma)};

    for (unsigned j = 0; j < n_quant_; ++j) {
      size = quantile(dist, AS_DOUBLE(quantiles_[j]));
      if (size < a1_)
        total += alpha_ * pow(2.0,-((size - a1_) / sigma_l_ * (size-a1_)/sigma_l_));
      else if (mean < (a1_ + a2_)) 
        total +=  alpha_;
      else 
        total += alpha_*pow(2.0,-((size-(a1_ + a2_))/sigma_r_ * (size-(a1_ + a2_))/sigma_r_));
    }
    return total / n_quant_;
  }
  LOG_CODE_ERROR() << "The specified distribution is not a valid distribution: " << dist;
  return 0;
}

} /* namespace selectivities */
} /* namespace niwa */
