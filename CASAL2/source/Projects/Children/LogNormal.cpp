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
#include "Model/Objects.h"

// namespaces
namespace niwa {
namespace projects {

/**
 * Default constructor
 */
LogNormal::LogNormal(Model* model) : Project(model) {
  parameters_.Bind<Double>(PARAM_MEAN, &mean_, "Mean of the lognormal distribution to be sampled", "", AS_DOUBLE(1.0));
  parameters_.Bind<Double>(PARAM_SIGMA, &sigma_, "Standard deviation on the log scale", "");
  parameters_.Bind<Double>(PARAM_MULTIPLIER, &multiplier_, "A multiplier to for ycs_values", "", AS_DOUBLE(1.0));
  parameters_.Bind<Double>(PARAM_RHO, &rho_, "an autocorrelation parameter on the log scale", "", AS_DOUBLE(0.0));

  RegisterAsEstimable(PARAM_SIGMA, &sigma_);
}

/**
 * Validate
 */
void LogNormal::DoValidate() {
  // TODO Validate so that year class values must be included or a default value for ycs for projection years

  if (sigma_ == 0)
    LOG_ERROR_P(PARAM_SIGMA) << " must not equal 0.0, Use the constant projection method if you would like to project a parameter with 0 variability";
  if (mean_ == 0.0)
    LOG_ERROR_P(PARAM_MEAN) << " should not be equal to zero on the log scale, causes -INF";
  if (multiplier_ == 0)
    LOG_ERROR_P(PARAM_MULTIPLIER)  << " should not be equal to zero";
}

/**
 * Build
 */

void LogNormal::DoBuild() {
  if (estimable_type_ != Estimable::kVector)
    LOG_ERROR() << "LogNormal projections can only be performed on parameters that are unsigned maps. if (estimable_type_ != Estimable::kUnsignedMap)";

  // if PARAM_RHO > 0 then we need to know the value of the ycs_value of first_random_year_ - 1, because the value for
  // first_random_year_ will be dependent on ycs_value of first_random_year_ - 1. We need access to the map, maybe through the assert method
  if (rho_ != 0.0) {
    //Double last_value_ = parameter_map[model_->current_year() - 1];
    //Double value = log((last_value - mean_) / (sigma_ * sigma_));

  }

}

/**
 * Reset
 */
void LogNormal::DoReset() { }

/**
 *  Update our parameter with a random draw from a lognormal distribution with specified inputs
 */
void LogNormal::DoUpdate() {
  // instance the random number generator
  if (rho_ != 0.0) {
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();
  Double Z = rng.normal(0.0, 1.0);
  value_ = -0.5 * sigma_ * sigma_ + sigma_ * (rho_ * last_value_ + sqrt(1 - rho_ * rho_ * Z));
  } else {
    utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();
    value_ = rng.lognormal(AS_DOUBLE(mean_), AS_DOUBLE(sigma_));
  }

  LOG_FINE() << "Setting Value to: " << value_;
  (this->*DoUpdateFunc_)(value_);
}

} /* namespace projects */
} /* namespace niwa */
