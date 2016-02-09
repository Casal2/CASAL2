/**
 * @file LogNormal.cpp
 * @author  Craig Marsh
 * @date 05/02/2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 */

// headers
#include "LogNormal.h"

#include "Utilities/RandomNumberGenerator.h"

// namespaces
namespace niwa {
namespace projects {

/**
 * Default constructor
 */
LogNormal::LogNormal(Model* model) : Project(model) {
  parameters_.Bind<Double>(PARAM_MEAN, &mean_, "Mean of the lognormal distribution to be sampled", "");
  parameters_.Bind<Double>(PARAM_CV, &cv_, "CV of the lognormal distribution to be sampled", "");
}

/**
 * Validate
 */
void LogNormal::DoValidate() {
  // TODO Validate so that year class values must be included or a default value for ycs for projection years
}

/**
 * Build
 */
void LogNormal::DoBuild() { }

/**
 * Reset
 */
void LogNormal::DoReset() { }

/**
 *  Update our parameter with a random draw from a lognormal distribution with specified inputs
 */
void LogNormal::DoUpdate() {
  // instance the random number generator
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();
  value_ = rng.lognormal(AS_DOUBLE(mean_), AS_DOUBLE(cv_));

  LOG_FINE() << "Setting Value to: " << value_;
  (this->*DoUpdateFunc_)(value_);
}

} /* namespace projects */
} /* namespace niwa */
