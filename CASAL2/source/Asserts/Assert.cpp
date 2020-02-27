/**
 * @file Assert.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 19/08/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
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
 * Note: The constructor is parsed to generate LaTeX for the documentation.
 */
Assert::Assert(Model* model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "The label for the assert", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "The type of the assert", "");
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when bind<>() overloads cannot be used
 *
 * Note: all parameters are populated from configuration files
 */
void Assert::Validate() {
  parameters_.Populate(model_);
  DoValidate();
}

} /* namespace niwa */
