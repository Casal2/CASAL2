/**
 * @file Increasing.cpp
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
#include "Increasing.h"

#include "Model/Model.h"

// Namespaces
namespace isam {
namespace selectivities {
/**
 * Default constructor
 */
Increasing::Increasing()
: Increasing(Model::Instance()) {

}

/**
 * Explicit constructor
 */
Increasing::Increasing(ModelPtr model)
: Selectivity(model) {
  parameters_.RegisterAllowed(PARAM_L);
  parameters_.RegisterAllowed(PARAM_H);
  parameters_.RegisterAllowed(PARAM_V);
  parameters_.RegisterAllowed(PARAM_ALPHA);
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
void Increasing::Validate() {
  LOG_TRACE();

  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_L);
  CheckForRequiredParameter(PARAM_H);
  CheckForRequiredParameter(PARAM_V);

  label_  = parameters_.Get(PARAM_LABEL).GetValue<string>();
  low_    = parameters_.Get(PARAM_L).GetValue<double>();
  high_   = parameters_.Get(PARAM_H).GetValue<double>();
  v_      = parameters_.Get(PARAM_V).GetValues<double, Double>();
  if (parameters_.IsDefined(PARAM_ALPHA))
    alpha_ = parameters_.Get(PARAM_ALPHA).GetValue<double>();

  if (alpha_ <= 0.0)
    LOG_ERROR(parameters_.location(PARAM_ALPHA) << ": alpha (" << AS_DOUBLE(alpha_) << ") cannot be less than or equal to 0.0");
  if (low_ < model_->min_age() || low_ > model_->max_age())
    LOG_ERROR(parameters_.location(PARAM_L) << ": 'l' (" << low_ << ") must be between the model min_age (" << model_->min_age() << ") and max_age (" << model_->max_age() << ")");
  if (high_ <= low_)
    LOG_ERROR(parameters_.location(PARAM_H) << ": 'h' (" << high_ << ") cannot be less than or the same as 'l' (" << low_ << ")");

  if (v_.size() != (high_ - low_ + 1)) {
    LOG_ERROR(parameters_.location(PARAM_V) << " 'v' has incorrect amount of elements\n"
        << "Expected: " << (high_ - low_ + 1) << " but got " << v_.size());
  }

  for (unsigned i = 0; i < v_.size(); ++i) {
    if (v_[i] < 0.0 || v_[i] > 1.0) {
      LOG_ERROR(parameters_.location(PARAM_V) << " 'v' element " << i + 1 << " (" << AS_DOUBLE(v_[i]) << ") is not between 0.0 and 1.0");
    }
  }

  RegisterAsEstimable(PARAM_V, v_);
}

/**
 * Reset this selectivity so it's ready for the next execution
 * phase in the model.
 *
 * This method will rebuild the cache of selectivity values
 * for each age in the model.
 */
void Increasing::Reset() {
  for (unsigned age = model_->min_age(); age <= model_->max_age(); ++age) {

    if (age < low_) {
      values_[age] = 0.0;

    } else if (age > high_) {
      values_[age] = *v_.rbegin();

    } else {
      Double value = *v_.begin();
      for (unsigned i = low_ + 1; i < age; ++i) {
        if (i > high_ || value >= alpha_)
          break;
        value += (alpha_ - value) * v_[i - low_];
      }

      values_[age] = value;
    }
  }
}

} /* namespace selectivities */
} /* namespace isam */
