/**
 * @file CasalPenalty.cpp
 * @author C.Marsh
 * @github https://github.com/Zaita
 * @date 4/9/15
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "CasalPenalty.h"

// namespaces
namespace niwa {
namespace additionalpriors {

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
CasalPenalty::CasalPenalty() {
  parameters_.Bind<Double>(PARAM_MULTIPLIER, &multiplier_, "Multiplier", "");
  parameters_.Bind<bool>(PARAM_LOG_SCALE, &log_scale_, "Log Scale", "");
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when we cannot use bind<>() overloads
 *
 * Note: all parameters are populated from configuration files
 */
void CasalPenalty::DoValidate() {

}

/**
 * TODO: Add documentation
 */
Double CasalPenalty::ratio_score() {
  return 0.0f;
}

/**
 * TODO: Add documentation
 */
Double CasalPenalty::difference_score() {
  return 0.0;
}

/**
 * TODO: Add documentation
 */
Double CasalPenalty::mean_score() {
  return 0.0;
}

} /* namespace additionalpriors */
} /* namespace niwa */
