/**
 * @file Profile.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/03/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Profile.h"

#include "Estimates/Children/Uniform.h"
#include "ObjectsFinder/ObjectsFinder.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {

namespace util = niwa::utilities;

/**
 * default constructor
 */
Profile::Profile() {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "", "");
  parameters_.Bind<unsigned>(PARAM_STEPS, &steps_, "The number of steps to take between the lower and upper bound", "");
  parameters_.Bind<Double>(PARAM_LOWER_BOUND, &lower_bound_, "The lower bounds", "");
  parameters_.Bind<Double>(PARAM_UPPER_BOUND, &upper_bound_, "The upper bounds", "");
  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "The system parameter to profile", "");
}

/**
 *
 */
void Profile::Validate() {
  parameters_.Populate();
}

/**
 *
 */
void Profile::Build() {
  string type       = "";
  string label      = "";
  string parameter  = "";
  string index      = "";

  /**
   * Explode the parameter string so we can get the estimable
   * name (parameter) and the index
   */
  objects::ExplodeString(parameter_, type, label, parameter, index);
  if (type == "" || label == "" || parameter == "") {
    LOG_ERROR_P(PARAM_PARAMETER) << ": parameter " << parameter_
        << " is not in the correct format. Correct format is object_type[label].estimable(array index)";
  }
  objects::ImplodeString(type, label, parameter, index, parameter_);

  base::ObjectPtr target = objects::FindObject(parameter_);
  if (!target) {
    LOG_ERROR_P(PARAM_PARAMETER) << ": parameter " << parameter_ << " is not a valid estimable in the system";
  }

  target_ = target->GetEstimable(parameter);
  original_value_ = *target_;

  step_size_ = (upper_bound_ - lower_bound_) / (steps_ + 1);
}

/**
 *
 */
void Profile::FirstStep() {
  *target_ = lower_bound_;
}
/**
 *
 */
void Profile::NextStep() {
  *target_ += step_size_;
}

void Profile::RestoreOriginalValue() {
  *target_ = original_value_;
}

} /* namespace niwa */
