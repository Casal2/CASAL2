/**
 * @file KnifeEdge.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "KnifeEdge.h"

#include <boost/math/distributions/lognormal.hpp>
#include <cmath>

#include "../../AgeLengths/AgeLength.h"
#include "../../Model/Model.h"
#include "../../TimeSteps/Manager.h"

// namespaces
namespace niwa {
namespace selectivities {

/**
 * Default Constructor
 */
KnifeEdge::KnifeEdge(shared_ptr<Model> model) : Selectivity(model) {
  parameters_.Bind<Double>(PARAM_E, &edge_, "The edge value", "");
  parameters_.Bind<Double>(PARAM_ALPHA, &alpha_, "The maximum value of the selectivity", "", 1.0);

  RegisterAsAddressable(PARAM_ALPHA, &alpha_);
  RegisterAsAddressable(PARAM_E, &edge_);
}

/**
 * Reset this selectivity so it is ready for the next execution
 * phase in the model.
 *
 * This method will rebuild the cache of selectivity values
 * for each age or length in the model.
 */
void KnifeEdge::RebuildCache() {
  if (model_->partition_type() == PartitionType::kAge) {
    for (unsigned age = model_->min_age(); age <= model_->max_age(); ++age) {
      Double temp = age * 1.0;
      if (temp >= edge_)
        values_[age - age_index_] = alpha_;
      else
        values_[age - age_index_] = 0.0;
    }
  } else if (model_->partition_type() == PartitionType::kLength) {
    vector<double> length_bins = model_->length_bins();

    for (unsigned length_bin_index = 0; length_bin_index < length_bins.size(); ++length_bin_index) {
      Double temp = (Double)length_bins[length_bin_index];
      if (temp >= edge_)
        length_values_[length_bin_index] = alpha_;
      else
        length_values_[length_bin_index] = 0.0;
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

Double KnifeEdge::GetLengthBasedResult(unsigned age, AgeLength* age_length, unsigned year, int time_step_index) {
  unsigned yearx     = year == 0 ? model_->current_year() : year;
  unsigned time_step = time_step_index == -1 ? model_->managers()->time_step()->current_time_step() : (unsigned)time_step_index;
  Double   cv        = age_length->cv(yearx, time_step, age);
  Double   mean      = age_length->mean_length(time_step, age);
  string   dist      = age_length->distribution_label();
  LOG_FINEST() << "age = " << age << " mean = " << mean << " cv " << cv << " dist = " << dist;
  if (dist == PARAM_NONE || n_quant_ <= 1) {
    // no distribution_label just use the mu from age_length
    if (mean >= edge_)
      return alpha_;
    else
      return 0.0;

  } else if (dist == PARAM_NORMAL) {
    Double sigma = cv * mean;
    Double size  = 0.0;
    Double total = 0.0;

    for (unsigned j = 0; j < n_quant_; ++j) {
      size = mean + sigma * quantiles_at_[j];

      if (size >= edge_)
        total += alpha_;
      else
        total += 0.0;
    }
    return total / n_quant_;

  } else if (dist == PARAM_LOGNORMAL) {
    // convert paramters to log space
    Double                 sigma = sqrt(log(1 + cv * cv));
    Double                 mu    = log(mean) - sigma * sigma * 0.5;
    Double                 size  = 0.0;
    Double                 total = 0.0;
    boost::math::lognormal dist{AS_DOUBLE(mu), AS_DOUBLE(sigma)};
    LOG_FINEST() << " mu = " << mu << " sigma = " << sigma << " total " << total << " edge = " << edge_;
    for (unsigned j = 0; j < n_quant_; ++j) {
      size = quantile(dist, AS_DOUBLE(quantiles_[j]));
      LOG_FINEST() << "j = " << j << " quant " << quantiles_[j] << " size = " << size;

      if (size >= edge_)
        total += alpha_;
      else
        total += 0.0;
    }
    return total / n_quant_;
  }
  LOG_CODE_ERROR() << "The specified distribution is not a valid distribution: " << dist;
  return 0;
}

} /* namespace selectivities */
} /* namespace niwa */
