/**
 * @file Profile.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/03/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */

// headers
#include "Profile.h"

#include "../Estimates/Common/Uniform.h"
#include "../Model/Model.h"
#include "../Model/Objects.h"
#include "../Utilities/To.h"

// namespaces
namespace niwa {

namespace util = niwa::utilities;

/**
 * Default constructor
 */
Profile::Profile(shared_ptr<Model> model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "The label of the profile", "", "");
  parameters_.Bind<unsigned>(PARAM_STEPS, &steps_, "The number of steps between the lower and upper bound", "")->set_lower_bound(2, true);
  parameters_.Bind<Double>(PARAM_LOWER_BOUND, &lower_bound_, "The lower value of the range", "");
  parameters_.Bind<Double>(PARAM_UPPER_BOUND, &upper_bound_, "The upper value of the range", "");
  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "The system parameter to profile", "");
  parameters_.Bind<string>(PARAM_SAME, &same_labels_, "List of other parameters that are constrained to have the same value as this parameter", "", "");
}

/**
 * Validate the objects
 */
void Profile::Validate() {
  parameters_.Populate(model_);
}

/**
 * Build the objects
 */
void Profile::Build() {
  string error = "";
  if (!model_->objects().VerifyAddressableForUse(parameter_, addressable::kProfile, error)) {
    LOG_FATAL_P(PARAM_PARAMETER) << "This parameter cannot be used in an @profile block. Error: " << error;
  }


  target_         = model_->objects().GetAddressable(parameter_);
  original_value_ = *target_;
  step_size_      = (upper_bound_ - lower_bound_) / (steps_ - 1);
  LOG_MEDIUM() << "start_value for parameter: " << original_value_;

  // Deal with Same parameters
  if (!parameters_.Get(PARAM_SAME)->has_been_defined()) {
    for (auto same : same_labels_) {
      if (!model_->objects().VerifyAddressableForUse(same, addressable::kProfile, error)) {
        LOG_FATAL_P(PARAM_SAME) << "This parameter cannot be used in an @profile block. Error: " << error;
      }

      auto same_target         = model_->objects().GetAddressable(same);
      if(!same_target) {
        LOG_FATAL_P(PARAM_SAME) << "Could nof find " << same << " does it exist, or in the right format?";
      }
      sames_.push_back(same_target);
      LOG_MEDIUM() << "start_value for same parameter: " << *same_target;
    }
  }
}
/**
 * Verify
 */
void Profile::Verify(shared_ptr<Model> model){
  LOG_MEDIUM() << "Verify ";
  LOG_FINE() << "check if used for transformation";
  if (model->objects().IsParameterUsedFor(parameter_ , addressable::kTransformation)) {
    LOG_FATAL_P(PARAM_PARAMETER) << "Found an @parameter_transformation block for " << parameter_ << ". You cannot have a @parameter_transformation and a @profile for the same parameter.";
  }
  if (model->objects().IsParameterUsedFor(parameter_ , addressable::kTimeVarying)) {
    LOG_FATAL_P(PARAM_PARAMETER) << "Found an @time_varying block for " << parameter_ << ". You cannot have a @time_varying and a @profile for the same parameter.";
  }
}
/**
 * Set up the first step of the profile by initialising the object to the lower bound
 */
void Profile::FirstStep() {
  *target_ = lower_bound_;
  values_.push_back(*target_);
  if (parameters_.Get(PARAM_SAME)->has_been_defined()) {
    for (Double* same : sames_) {
      *same = lower_bound_;
      LOG_MEDIUM() << "Profiling with profile parameter = " << *target_ << " and same parameter = " << *same;
    }
  }
}
/**
 * Set up the next step of the profile by incrementing the object by the step size
 */
void Profile::NextStep() {
  *target_ += step_size_;
  values_.push_back(*target_);
  if (parameters_.Get(PARAM_SAME)->has_been_defined()) {
    for (Double* same : sames_) {
      *same += step_size_;
      LOG_MEDIUM() << "Profiling with profile parameter = " << *target_ << " and same parameter = " << *same;
    }
  }
}

/**
 * Restore the original value of the object
 */
void Profile::RestoreOriginalValue() {
  *target_ = original_value_;
  if (parameters_.Get(PARAM_SAME)->has_been_defined()) {
    for (Double* same : sames_) {
      *same = original_value_;
      LOG_MEDIUM() << "Profiling with profile parameter = " << *target_ << " and same parameter = " << *same;
    }
  }
}

} /* namespace niwa */
