/**
 * @file Assert.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 19/08/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */

// headers
#include "Assert.h"

// namespaces
namespace niwa {

/**
 * Default constructor
 *
 * Bind any parameters that are allowed to be loaded from the configuration files.
 * Set bounds on registered parameters
 * Register any parameters that can be an estimated or utilised in other run modes (e.g., profiling, yields, projections, etc.)
 * Set some initial values
 *
 * Note: The constructor is parsed to generate Latex for the documentation.
 */
Assert::Assert(shared_ptr<Model> model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "The label for the assert", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "The type of the assert", "")->set_allowed_values({PARAM_ADDRESSABLE, PARAM_OBJECTIVE_FUNCTION, PARAM_PARTITION});
  parameters_.Bind<Double>(PARAM_TOLERANCE, &tol_, "Tolerance", "", 1e-5);
  parameters_.Bind<string>(PARAM_ERROR_TYPE, &error_type_, "Report assert failures as either an error or warning", "", PARAM_ERROR)
      ->set_allowed_values({PARAM_ERROR, PARAM_WARNING});
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when we cannot use bind<>() overloads
 *
 * Note: all parameters are populated from configuration files
 */
void Assert::Validate() {
  parameters_.Populate(model_);
  DoValidate();
}

} /* namespace niwa */
