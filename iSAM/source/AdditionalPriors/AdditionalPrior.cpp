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
 *
 * Bind any parameters that are allowed to be loaded from the configuration files.
 * Set bounds on registered parameters
 * Register any parameters that can be an estimated or utilised in other run modes (e.g profiling, yields, projections etc)
 * Set some initial values
 *
 * Note: The constructor is parsed to generate Latex for the documentation.
 */
AdditionalPrior::AdditionalPrior() {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type", "");
  parameters_.Bind<string>(PARAM_METHOD, &method_, "Method", "")
    ->set_allowed_values({PARAM_RATIO, PARAM_DIFFERENCE, PARAM_MEAN});
}

/**
 * Return the score from the additional prior
 *
 * @return Score from additional prior
 */
Double AdditionalPrior::GetScore() {
  if (DoScoreFunction_ == 0)
    LOG_CODE_ERROR() << "DoScoreFunction_ == 0";
  return (this->*DoScoreFunction_)();
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when we cannot use bind<>() overloads
 *
 * Note: all parameters are populated from configuration files
 */
void AdditionalPrior::Validate() {
  parameters_.Populate();
  DoValidate();

  // assign our function pointer
  if (method_ == PARAM_RATIO)
    DoScoreFunction_ = &AdditionalPrior::GetRatioScore;
  else if (method_ == PARAM_DIFFERENCE)
    DoScoreFunction_ = &AdditionalPrior::GetDifferenceScore;
  else
    DoScoreFunction_ = &AdditionalPrior::GetMeanScore;
}

} /* namespace niwa */
