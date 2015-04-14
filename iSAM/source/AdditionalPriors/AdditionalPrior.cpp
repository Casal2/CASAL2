/**
 * @file AdditionalPrior.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "AdditionalPrior.h"

// namespaces
namespace niwa {

/**
 * Default constructor
 */
AdditionalPrior::AdditionalPrior() {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type", "");
  parameters_.Bind<string>(PARAM_METHOD, &method_, "Method", "")
    ->set_allowed_values({PARAM_RATIO, PARAM_DIFFERENCE, PARAM_MEAN});
}

/**
 *
 */
Double AdditionalPrior::GetScore() {
  if (DoScoreFunction_ == 0)
    LOG_CODE_ERROR() << "DoScoreFunction_ == 0";
  return (this->*DoScoreFunction_)();
}

/**
 *
 */
void AdditionalPrior::Validate() {
  parameters_.Populate();
  DoValidate();

  if (method_ != PARAM_RATIO && method_ != PARAM_DIFFERENCE && method_ != PARAM_MEAN)
    LOG_ERROR_P(PARAM_METHOD) << "must be either ratio, difference or mean." << method_ << " is not supported";
  // assign our function pointer
  if (method_ == PARAM_RATIO)
    DoScoreFunction_ = &AdditionalPrior::GetRatioScore;
  else if (method_ == PARAM_DIFFERENCE)
    DoScoreFunction_ = &AdditionalPrior::GetDifferenceScore;
  else
    DoScoreFunction_ = &AdditionalPrior::GetMeanScore;
}

} /* namespace niwa */
