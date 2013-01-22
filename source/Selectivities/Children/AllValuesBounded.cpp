/**
 * @file AllValuesBounded.cpp
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
#include "AllValuesBounded.h"

#include "Model/Model.h"

// Namespaces
namespace isam {
namespace selectivities {

/**
 * Default constructor
 */
AllValuesBounded::AllValuesBounded()
: AllValuesBounded(Model::Instance()) {
}

/**
 * Explicit Constructor
 */
AllValuesBounded::AllValuesBounded(ModelPtr model)

: Selectivity(model) {
  parameters_.RegisterAllowed(PARAM_L);
  parameters_.RegisterAllowed(PARAM_H);
  parameters_.RegisterAllowed(PARAM_V);
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
void AllValuesBounded::Validate() {
  LOG_TRACE();

  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_L);
  CheckForRequiredParameter(PARAM_H);
  CheckForRequiredParameter(PARAM_V);

  unsigned min_age = model_->min_age();
  unsigned max_age = model_->max_age();

  label_  = parameters_.Get(PARAM_LABEL).GetValue<string>();

  // Param: L
  low_ = parameters_.Get(PARAM_L).GetValue<unsigned>();
  if (low_ < min_age) {
    LOG_ERROR(parameters_.location(PARAM_L) << ": Parameter 'l' is less than the 'min_age' for the model\n"
        << "Model 'min_age' is " << min_age << " and 'l' is " << low_);
  }

  // Param: H
  high_ = parameters_.Get(PARAM_H).GetValue<unsigned>();
  if (high_ > max_age) {
    LOG_ERROR(parameters_.location(PARAM_H) << ": Parameter 'h' is greater than the 'max_age' for the model\n"
        << "Model 'max_age' is " << max_age << " and 'h' is " << high_);
  }

  if (low_ >= high_) {
    LOG_ERROR(parameters_.location(PARAM_L) << ": Parameter 'l' is greater than or equal to parameter 'h'\n"
        << "'l' = " << low_ << " and 'h' = " << high_);
  }

  // Param: V
  v_ = parameters_.Get(PARAM_V).GetValues<double>();
  if (v_.size() != (high_ - low_)) {
    LOG_ERROR(parameters_.location(PARAM_V) << ": Parameter 'v' does not have the right amount of elements n = h - l\n"
        << "Expected " << high_ - low_ << " but got " << v_.size());
  }

  // TODO: Register v_ as estimable
}

/**
 * Reset this selectivity so it's ready for the next execution
 * phase in the model.
 *
 * This method will rebuild the cache of selectivity values
 * for each age in the model.
 */
void AllValuesBounded::Reset() {
  unsigned min_age = model_->min_age();

  /**
   * Resulting age map should look like
   * While Age < Low :: Value = 0.0
   * While Age > Low && Age < High :: Value = v_
   * While age > High :: Value = Last element if v_
   */
  for (unsigned age = min_age; age < low_; ++age)
    values_[age] = 0.0;
  for (unsigned i = 0; i < v_.size(); ++i)
    values_[i + min_age] = v_[i];
  for (unsigned age = min_age + v_.size(); age < high_; ++age)
    values_[age] = *v_.rbegin();
}

} /* namespace selectivities */
} /* namespace isam */
