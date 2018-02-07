/**
 * @file AgeWeight.cpp
 * @author  C.Marsh
 * @date 16/11/2017
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 *
 */

// defines
#define _USE_MATH_DEFINES

// headers
#include "../AgeWeights/AgeWeight.h"

#include <cmath>

#include "Common/Model/Managers.h"
#include "Common/TimeSteps/Manager.h"
#include "Common/Utilities/Map.h"
#include "Common/Estimates/Manager.h"


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
AgeWeight::AgeWeight(Model* model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label of the age weight relationship", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type of age weight class", "");

}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when we cannot use bind<>() overloads
 *
 * Note: all parameters are populated from configuration files
 */
void AgeWeight::Validate() {
  LOG_TRACE();
  parameters_.Populate(model_);
  DoValidate();

}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 */
void AgeWeight::Build() {
  LOG_TRACE();
  DoBuild();
}

/**
 * Reset the age length class.
 */
void AgeWeight::Reset() {
  LOG_TRACE();
  DoReset();
}

/**
 * ReBuild Cache: intiated by the time_varying class.
 */
void AgeWeight::RebuildCache() {
  LOG_TRACE();

}

} /* namespace niwa */
