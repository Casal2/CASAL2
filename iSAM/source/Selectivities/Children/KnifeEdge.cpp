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

#include <boost/math/distributions/lognormal.hpp>
#include <cmath>

#include "AgeLengths/AgeLength.h"
#include "Model/Model.h"
// namespaces
namespace niwa {
namespace selectivities {

/**
 * Explicit Constructor
 */
KnifeEdge::KnifeEdge(Model* model)
  : Selectivity(model) {

  parameters_.Bind<Double>(PARAM_E, &edge_, "Edge", "");
  parameters_.Bind<Double>(PARAM_ALPHA, &alpha_, "Alpha", "", 1.0);

  RegisterAsEstimable(PARAM_ALPHA, &alpha_);
  RegisterAsEstimable(PARAM_E, &edge_);
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
    Double temp = age * 1.0;
    if (temp >= edge_)
      values_[age] = alpha_;
    else
      values_[age] = 0.0;
  }
}

/**
 * GetLengthBasedResult function
 *
 * @param age
 * @param age_length AgeLength pointer
 * @return Double selectivity for an age based on age length distribution
 */

Double KnifeEdge::GetLengthBasedResult(unsigned age, AgeLength* age_length) {
  Double cv = age_length->cv(age);
  unsigned year = model_->current_year();
  Double mean = age_length->mean_length(year, age);
  string dist = age_length->distribution();

  if (dist == PARAM_NONE || n_quant_ <= 1) {
    // no distribution just use the mu from age_length
    if (mean >= edge_)
      return alpha_;
    else
      return 0.0;

  } else if (dist == PARAM_NORMAL) {

    Double sigma = cv * mean;
    Double size = 0.0;
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
    Double sigma = sqrt(log(1 + cv * cv));
    Double mu = log(mean) - sigma * sigma * 0.5;
    Double size = 0.0;
    Double total = 0.0;
    boost::math::lognormal dist{ mu, sigma };

    for (unsigned j = 0; j < n_quant_; ++j) {
      size = mu + sigma * quantile(dist, quantiles_[j]);

      if (size >= edge_)
        total += alpha_;
      else
        total += 0.0;
    }
    return total / n_quant_;
  }
  LOG_CODE_ERROR() << "dist is invalid " << dist;
  return 0;
}


} /* namespace selectivities */
} /* namespace niwa */
