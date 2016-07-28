/**
 * @file Estimate.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 8/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Estimate.h"

#include "Utilities/DoubleCompare.h"

// Namespaces
namespace niwa {

/**
 * Default constructor
 */
Estimate::Estimate() {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type", "");
  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "The name of the variable to estimate in the model", "");
  parameters_.Bind<Double>(PARAM_LOWER_BOUND, &lower_bound_, "The lowest value the parameter is allowed to have", "");
  parameters_.Bind<Double>(PARAM_UPPER_BOUND, &upper_bound_, "The highest value the parameter is allowed to have", "");
  parameters_.Bind<string>(PARAM_PRIOR, &prior_label_, "The name of the prior to use for the parameter", "", "");
  parameters_.Bind<string>(PARAM_SAME, &same_labels_, "A list of parameters that are bound to the value of this estimate", "", "");
  parameters_.Bind<unsigned>(PARAM_ESTIMATION_PHASE, &estimation_phase_, "TBA", "", 1);
  parameters_.Bind<bool>(PARAM_MCMC, &mcmc_fixed_, "This parameter fixes parameters during an MCMC run", "", false);
}

/**
 * Validate our estimate. Some of the
 * validation was done by the
 * estimates::Info object before the
 * estimate was created so we can skip that.
 */
void Estimate::Validate() {
  if (*target_ < lower_bound_)
    LOG_ERROR() << location() <<  "the initial value(" << AS_DOUBLE((*target_)) << ") on the estimate " << parameter_
        << " is lower than the lower_bound(" << lower_bound_ << ")";
  if (*target_ > upper_bound_)
    LOG_ERROR() << location() << "the initial value(" << AS_DOUBLE((*target_)) << ") on the estimate " << parameter_
        << " is greater than the upper_bound(" << upper_bound_ << ")";

  DoValidate();
}

/**
 *
 */
void Estimate::Reset() {
  /**
   * Reset the value if the bounds are the same so we can ensure all of the
   * "same" parameters are aligned
   */
  if (utilities::doublecompare::IsEqual(lower_bound_, upper_bound_))
    set_value(value());
}

/**
 * This method will add all of the "sames" to this object.
 * We add the labels as well for reporting and debugging
 * purposes.
 *
 * @param label The label of the same to add
 * @param target The target value to modify when we set a new value
 */
void Estimate::AddSame(const string& label, Double* target) {
  same_labels_.push_back(label);
  sames_.push_back(target);
}

/**
 * Assign a new value to the object pointed to by
 * this estimate. We will also iterate over any
 * "sames" and assign the value to them as well.
 *
 * @param new_value The new value to assign.
 */
void Estimate::set_value(Double new_value) {
  *target_ = new_value;
  for (Double* same : sames_)
    *same = new_value;
}

} /* namespace niwa */
