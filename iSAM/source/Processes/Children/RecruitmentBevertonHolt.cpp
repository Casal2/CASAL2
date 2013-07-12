/**
 * @file RecruitmentBevertonHolt.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 12/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "RecruitmentBevertonHolt.h"

// namespaces
namespace isam {
namespace processes {

/**
 * default constructor
 */
RecruitmentBevertonHolt::RecruitmentBevertonHolt() {
  LOG_TRACE();

  parameters_.RegisterAllowed(PARAM_LABEL);
  parameters_.RegisterAllowed(PARAM_CATEGORIES);
  parameters_.RegisterAllowed(PARAM_R0);
  parameters_.RegisterAllowed(PARAM_PROPORTIONS);
  parameters_.RegisterAllowed(PARAM_AGE);
  parameters_.RegisterAllowed(PARAM_STEEPNESS);
  parameters_.RegisterAllowed(PARAM_SSB);
  parameters_.RegisterAllowed(PARAM_B0);
  parameters_.RegisterAllowed(PARAM_SSB_OFFSET);
  parameters_.RegisterAllowed(PARAM_YCS_VALUES);
  parameters_.RegisterAllowed(PARAM_STANDARDISE_YCS_YEARS);

  RegisterAsEstimable(PARAM_R0, &r0_);
  RegisterAsEstimable(PARAM_STEEPNESS, &steepness_);
}

/**
 *
 */
void RecruitmentBevertonHolt::Validate() {
  model_ = Model::Instance();

  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_CATEGORIES);

  label_          = parameters_.Get(PARAM_LABEL).GetValue<string>();
  category_names_ = parameters_.Get(PARAM_CATEGORIES).GetValues<string>();
  r0_             = parameters_.Get(PARAM_R0).GetValue<double>();
  age_            = parameters_.Get(PARAM_AGE).GetValue<unsigned>(model_->min_age());
  steepness_      = parameters_.Get(PARAM_STEEPNESS).GetValue<double>(1.0);
  ssb_            = parameters_.Get(PARAM_SSB).GetValue<string>();
  ssb_offset_     = parameters_.Get(PARAM_SSB_OFFSET).GetValue<unsigned>();

  proportions_    = parameters_.Get(PARAM_PROPORTIONS).GetValues<Double>();

  // Ensure defined categories were valid
//  for(const string& category : category_names_) {
//    if (!Categories::Instance()->IsValid(category))
//      LOG_ERROR(parameters_.location(PARAM_CATEGORIES) << ": category " << category << " is not a valid category");
//  }
}

/**
 *
 */
void RecruitmentBevertonHolt::Build() {

}

/**
 *
 */
void RecruitmentBevertonHolt::Execute() {

}

} /* namespace processes */
} /* namespace isam */
