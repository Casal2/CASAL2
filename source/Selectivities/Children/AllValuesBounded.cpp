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
AllValuesBounded::AllValuesBounded() {

  parameters_.RegisterAllowed(PARAM_L);
  parameters_.RegisterAllowed(PARAM_H);
  parameters_.RegisterAllowed(PARAM_V);
}

/**
 * Validate this selectivity
 */
void AllValuesBounded::Validate() {
  LOG_TRACE();

  CheckForRequiredParameter(PARAM_L);
  CheckForRequiredParameter(PARAM_H);
  CheckForRequiredParameter(PARAM_V);

  ModelPtr model = Model::Instance();
  unsigned min_age = model->min_age();
  unsigned max_age = model->max_age();

  // Param: L
  low_ = parameters_.Get(PARAM_L).GetValue<unsigned>();
  if (low_ < min_age) {
    Parameter parameter = parameters_.Get(PARAM_L);
    LOG_ERROR(parameter.location() << ": Parameter 'l' is less than the 'min_age' for the model\n"
        << "Model 'min_age' is " << min_age << " and 'l' is " << low_);
  }

  // Param: H
  high_ = parameters_.Get(PARAM_H).GetValue<unsigned>();
  if (high_ > max_age) {
    Parameter parameter = parameters_.Get(PARAM_H);
    LOG_ERROR(parameter.location() << ": Parameter 'h' is greater than the 'max_age' for the model\n"
        << "Model 'max_age' is " << max_age << " and 'h' is " << high_);
  }

  if (low_ >= high_) {
    Parameter parameter = parameters_.Get(PARAM_L);
    LOG_ERROR(parameter.location() << ": Parameter 'l' is greater than or equal to parameter 'h'\n"
        << "'l' = " << low_ << " and 'h' = " << high_);
  }

  // Param: V
  v_ = parameters_.Get(PARAM_V).GetValues<double>();
  if (v_.size() != (high_ - low_)) {
    Parameter parameter = parameters_.Get(PARAM_V);
    LOG_ERROR(parameter.location() << ": Parameter 'v' does not have the right amount of elements n = h - l\n"
        << "Expected " << high_ - low_ << " but got " << v_.size());
  }

  // TODO: Register all v values as estimable
}

/**
 * Return the correct result for this selectivity from the cache.
 * Any age_or_length below low_ will return 0.0
 * Any age_or_length above high_ will return the last v_ value
 *
 * @parameter age_or_length The age or length to get the value for
 * @return The value from the cache
 */
Double AllValuesBounded::GetResult(unsigned age_or_length) {

  if (age_or_length < low_)
    return 0.0;

  if (age_or_length > high_)
    return values_[high_];

  return values_[age_or_length];
}

} /* namespace selectivities */
} /* namespace isam */
