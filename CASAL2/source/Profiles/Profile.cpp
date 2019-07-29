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

#include "Estimates/Common/Uniform.h"
#include "Model/Model.h"
#include "Model/Objects.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {

namespace util = niwa::utilities;

/**
 * default constructor
 */
Profile::Profile(Model* model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "", "");
  parameters_.Bind<unsigned>(PARAM_STEPS, &steps_, "The number of steps to take between the lower and upper bound", "");
  parameters_.Bind<Double>(PARAM_LOWER_BOUND, &lower_bound_, "The lower bounds", "");
  parameters_.Bind<Double>(PARAM_UPPER_BOUND, &upper_bound_, "The upper bounds", "");
  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "The system parameter to profile", "");
  parameters_.Bind<string>(PARAM_SAME, &same_parameter_, "A Parameter that are constrained to have the same value as the parameter being profiled", "", "");
}

/**
 *
 */
void Profile::Validate() {
  parameters_.Populate(model_);
}

/**
 *
 */
void Profile::Build() {
  string error = "";
  if (!model_->objects().VerfiyAddressableForUse(parameter_, addressable::kProfile, error)) {
    LOG_FATAL_P(PARAM_PARAMETER) << "could not be verified for use in a @profile block. Error was " << error;
  }

  target_ = model_->objects().GetAddressable(parameter_);
  original_value_ = *target_;
  step_size_ = (upper_bound_ - lower_bound_) / (steps_ + 1);
  LOG_MEDIUM() << "start_value for parameter: " << original_value_;

  /**
   * Deal with the same parameter
   */
  if (same_parameter_ != "") {
    if (!model_->objects().VerfiyAddressableForUse(same_parameter_, addressable::kProfile, error)) {
      LOG_FATAL_P(PARAM_SAME) << "could not be verified for use in a @profile block. Error was " << error;
    }

    same_target_ = model_->objects().GetAddressable(same_parameter_);
    same_original_value_ = *same_target_;
    LOG_MEDIUM() << "start_value for same parameter: " << same_original_value_;
  }
}

/**
 *
 */
void Profile::FirstStep() {
  *target_ = lower_bound_;
  if (parameters_.Get(PARAM_SAME)->has_been_defined()) {
    *same_target_ = lower_bound_;
    LOG_MEDIUM() << "Profiling with profile parameter = " <<  *target_  << " and same parameter = " << *same_target_;
  }
}
/**
 *
 */
void Profile::NextStep() {
  *target_ += step_size_;
  if (parameters_.Get(PARAM_SAME)->has_been_defined()) {
    *same_target_ += step_size_;
    LOG_MEDIUM() << "Profiling with profile parameter = " <<  *target_  << " and same parameter = " << *same_target_;
  }
}

void Profile::RestoreOriginalValue() {
  *target_ = original_value_;
  if (parameters_.Get(PARAM_SAME)->has_been_defined()) {
    *same_target_ = original_value_;
    LOG_MEDIUM() << "Profiling with profile parameter = " <<  *target_  << " and same parameter = " << *same_target_;
  }
}

} /* namespace niwa */
