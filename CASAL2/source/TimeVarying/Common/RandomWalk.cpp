/**
 * @file RandomWalk.cpp
 * @author Craig Marsh
 * @github https://github.com/Zaita
 * @date 2/02/2016
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */

// headers
#include "RandomWalk.h"

#include "../../Estimates/Manager.h"
#include "../../Model/Objects.h"
#include "../../Utilities/Map.h"
#include "../../Utilities/RandomNumberGenerator.h"

// namespaces
namespace niwa {
namespace timevarying {

/**
 * Default constructor
 */
RandomWalk::RandomWalk(shared_ptr<Model> model) : TimeVarying(model) {
  parameters_.Bind<Double>(PARAM_MEAN, &mu_, "The mean (mu)", "", 0);
  parameters_.Bind<Double>(PARAM_SIGMA, &sigma_, "The standard deviation (sigma)", "", 1);
  parameters_.Bind<Double>(PARAM_UPPER_BOUND, &upper_bound_, "The upper bound for the random walk", "", 1);
  parameters_.Bind<Double>(PARAM_UPPER_BOUND, &lower_bound_, "The lower bound for the random walk", "", 1);
  parameters_.Bind<Double>(PARAM_RHO, &rho_, "The autocorrelation parameter (rho)", "", 1);
  parameters_.Bind<string>(PARAM_DISTRIBUTION, &distribution_, "The distribution", "", PARAM_NORMAL);

  RegisterAsAddressable(PARAM_MEAN, &mu_);
  RegisterAsAddressable(PARAM_SIGMA, &sigma_);
}

/**
 * Validate
 */
void RandomWalk::DoValidate() {
  if (distribution_ != PARAM_NORMAL)
    LOG_ERROR() << "Random walk can draw from a normal distribution only";
}

/**
 *
 */
void RandomWalk::DoBuild() {
  // Warn users that they have a time-varying parameter in estimation mode.
  if (model_->run_mode() == RunMode::kEstimation) {
    LOG_WARNING() << "Time varying of type " << type_
                  << " during estimation, Not the correct implementation of a random effect. Its purpose is for simulating/projecting adding variaion.";
  }
  if (model_->objects().GetAddressableType(parameter_) != addressable::kSingle)
    LOG_ERROR_P(PARAM_TYPE) << "@time_varying blocks of type " << PARAM_RANDOMWALK << " can be used only with parameters that are scalars or single values";
}

/**
 * Update
 */
void RandomWalk::DoUpdate() {
  LOG_FINEST() << "value = " << *addressable_;
  utilities::RandomNumberGenerator& rng     = utilities::RandomNumberGenerator::Instance();
  Double                            value   = *addressable_;
  double                            deviate = rng.normal(AS_DOUBLE(mu_), AS_DOUBLE(sigma_));
  value += value * rho_ + deviate;

  if (value < lower_bound_) {
    LOG_FINEST() << "@estimate at lower bound, changing value from " << value << " to " << lower_bound_;
    value = lower_bound_;
  }
  if (value > upper_bound_) {
    LOG_FINEST() << "@estimate at upper bound, changing value from " << value << " to " << upper_bound_;
    value = upper_bound_;
  }

  LOG_FINEST() << "value after deviate of " << deviate << " = " << value << " for year " << model_->current_year();

  LOG_FINE() << "Setting Value to: " << value;
  parameter_by_year_[model_->current_year()] = value;
  (this->*update_function_)(value);
}

} /* namespace timevarying */
} /* namespace niwa */
