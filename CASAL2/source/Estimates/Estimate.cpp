/**
 * @file Estimate.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 8/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Estimate.h"

#include "../Model/Factory.h"
#include "../Model/Model.h"
#include "../Utilities/Math.h"
#include "../Utilities/To.h"

// Namespaces
namespace niwa {

/**
 * Default constructor
 */
Estimate::Estimate(shared_ptr<Model> model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "The label of the estimate", "", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "The type of prior for the estimate", "");
  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "The name of the parameter to estimate", "");
  parameters_.Bind<Double>(PARAM_LOWER_BOUND, &lower_bound_, "The lower bound for the parameter", "");
  parameters_.Bind<Double>(PARAM_UPPER_BOUND, &upper_bound_, "The upper bound for the parameter", "");
  parameters_.Bind<string>(PARAM_SAME, &same_labels_, "List of other parameters that are constrained to have the same value as this parameter", "", "");
  parameters_.Bind<unsigned>(PARAM_ESTIMATION_PHASE, &estimation_phase_, "The first estimation phase to allow this to be estimated", "", 1);
  parameters_.Bind<bool>(PARAM_MCMC_FIXED, &mcmc_fixed_, "Indicates if this parameter is estimated at the point estimate but fixed during MCMC estimation run", "", false);
}

/**
 * Validate the estimate. Some of the validation was done by the
 * estimates::Info object before the estimate was created so that can be skipped.
 * estimates::Info object before the
 * estimate was created so we can skip that.
 */
void Estimate::Validate() {
  DoValidate();
}

/**
 * Build the estimate
 */
void Estimate::Build() {
  if(!value_been_initialised_)
    set_initial_value(value());
  Reset();
}

/**
 * Reset the estimate
 */
void Estimate::Reset() {
  /**
   * Reset the value if the bounds are the same to ensure that all of the
   * "same" parameters are aligned
   */
  if (utilities::math::IsEqual(lower_bound_, upper_bound_))
    set_value(value());
  
}

/**
 * This method will add all of the "sames" to this object.
 * The labels are added as well for reporting and debugging
 * purposes.
 *
 * @param label The label of the same to add
 * @param target The target value to modify when setting a new value
 */
void Estimate::AddSame(const string& label, Double* target) {
  same_labels_.push_back(label);
  sames_.push_back(target);
}

/**
 * Assign a new value to the object pointed to by
 * this estimate. Also iterate over any
 * "sames" and assign the value to them as well.
 *
 * @param new_value The new value to assign.
 */
void Estimate::set_value(Double new_value) {
  *target_ = new_value;
  for (Double* same : sames_) *same = new_value;
}

} /* namespace niwa */
