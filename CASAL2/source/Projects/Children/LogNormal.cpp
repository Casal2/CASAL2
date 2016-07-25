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
  parameters_.Bind<Double>(PARAM_MEAN, &mean_, "Mean of the lognormal distribution to be sampled", "", 1.0);
  parameters_.Bind<Double>(PARAM_SIGMA, &sigma_, "Standard deviation on the log scale", "");
  parameters_.Bind<Double>(PARAM_RHO, &rho_, "an autocorrelation parameter on the log scale", "", 0.0);

}

/**
 * Validate
 */
void LogNormal::DoValidate() {
  if (sigma_ == 0)
    LOG_ERROR_P(PARAM_SIGMA) << " must not equal 0.0, Use the constant projection method if you would like to project a parameter with 0 variability";
  if (mean_ == 0.0)
    LOG_ERROR_P(PARAM_MEAN) << " should not be equal to zero on the log scale = -INF";
  if (multiplier_ == 0)
    LOG_ERROR_P(PARAM_MULTIPLIER)  << " should not be equal to zero";
}

/**
 * Build
 */

void LogNormal::DoBuild() {
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
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();
  LOG_TRACE();
  if (parameters_.Get(PARAM_RHO)->has_been_defined()) {
    LOG_FINE() << "Calculating an AR(1) process for projections.";
    Double last_value_ = projected_parameters_[model_->current_year() - 1];
    Double Z = rng.normal(0.0, 1.0);
    value_ = -0.5 * sigma_ * sigma_ + sigma_ * (rho_ * last_value_ + sqrt(1 - rho_ * rho_ * Z));
  } else {
    // Just a standard normal deviation
    value_ = rng.lognormal(AS_DOUBLE(mean_), AS_DOUBLE(sigma_));
  }
  // Store this value to be pulled out next projection year
  projected_parameters_[model_->current_year()] = value_;

  LOG_FINE() << "Setting Value to: " << value_;
  (this->*DoUpdateFunc_)(value_);
}

} /* namespace projects */
} /* namespace niwa */
