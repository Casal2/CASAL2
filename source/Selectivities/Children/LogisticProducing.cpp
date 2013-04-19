/**
 * @file LogisticProducing.cpp
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
#include "LogisticProducing.h"

#include <cmath>

#include "Model/Model.h"

// namespaces
namespace isam {
namespace selectivities {

/**
 * Default constructor
 */
LogisticProducing::LogisticProducing()
: LogisticProducing(Model::Instance()) {

}

/**
 * Explicit constructor
 */
LogisticProducing::LogisticProducing(ModelPtr model)
: Selectivity(model) {
  parameters_.RegisterAllowed(PARAM_L);
  parameters_.RegisterAllowed(PARAM_H);
  parameters_.RegisterAllowed(PARAM_A50);
  parameters_.RegisterAllowed(PARAM_ATO95);
  parameters_.RegisterAllowed(PARAM_ALPHA);

  RegisterAsEstimable(PARAM_A50, &a50_);
  RegisterAsEstimable(PARAM_ATO95, &aTo95_);
  RegisterAsEstimable(PARAM_ALPHA, &alpha_);
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
void LogisticProducing::Validate() {
  LOG_TRACE();

  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_L);
  CheckForRequiredParameter(PARAM_H);
  CheckForRequiredParameter(PARAM_A50);
  CheckForRequiredParameter(PARAM_ATO95);

  label_  = parameters_.Get(PARAM_LABEL).GetValue<string>();
  low_    = parameters_.Get(PARAM_L).GetValue<unsigned>();
  high_   = parameters_.Get(PARAM_H).GetValue<unsigned>();
  a50_    = parameters_.Get(PARAM_A50).GetValue<double>();
  aTo95_  = parameters_.Get(PARAM_ATO95).GetValue<double>();
  if (parameters_.IsDefined(PARAM_ALPHA))
    alpha_ = parameters_.Get(PARAM_ALPHA).GetValue<double>();

  if (alpha_ <= 0.0)
    LOG_ERROR(parameters_.location(PARAM_ALPHA) << ": alpha (" << alpha_ << ") cannot be less than or equal to 0.0");
  if (aTo95_ <= 0.0)
    LOG_ERROR(parameters_.location(PARAM_ATO95) << ": ato95 (" << aTo95_ << ") cannot be less than or equal to 0.0");
}

/**
 * Reset this selectivity so it's ready for the next execution
 * phase in the model.
 *
 * This method will rebuild the cache of selectivity values
 * for each age in the model.
 */
void LogisticProducing::Reset() {
  for (unsigned age = model_->min_age(); age <= model_->max_age(); ++age) {

    if (age < low_)
      values_[age] = 0.0;
    else if (age >= high_)
      values_[age] = alpha_;
    else if (age == low_)
      values_[age] = 1.0 / (1.0 + pow(19.0, (a50_ - age) / aTo95_)) * alpha_;
    else {
      Double lambda2 = 1.0 / (1.0 + pow(19.0, (a50_- (age - 1)) / aTo95_));
      if (lambda2 > 0.9999)
        values_[age] = alpha_;
      Double lambda1 = 1.0 / (1.0 + pow(19.0, (a50_ - age) / aTo95_));
      values_[age] = (lambda1 - lambda2) / (1 - lambda2) * alpha_;
    }
  }
}

} /* namespace selectivities */
} /* namespace isam */
