/*
 * EstimateTransformation.cpp
 *
 *  Created on: Dec 7, 2015
 *      Author: Zaita
 */

#include "EstimateTransformation.h"

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
EstimateTransformation::EstimateTransformation(Model* model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label for the transformation", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type of transformation", "");
  parameters_.Bind<Double>(PARAM_LOWER_BOUND, &lower_bound_, "Lower bound to use for the transformed estimate", "", Double(0.0));
  parameters_.Bind<Double>(PARAM_UPPER_BOUND, &upper_bound_, "Upper bound to use for the transformed estimate", "", Double(0.0));
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when we cannot use bind<>() overloads
 *
 * Note: all parameters are populated from configuration files
 */
void EstimateTransformation::Validate() {
  parameters_.Populate();
  DoValidate();
}

} /* namespace niwa */
