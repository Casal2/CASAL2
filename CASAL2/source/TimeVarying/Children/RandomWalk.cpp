/**
 * @file RandomWalk.cpp
 * @author Craig Marsh
 * @github https://github.com/Zaita
 * @date 2/02/2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "RandomWalk.h"

#include "Utilities/Map.h"
#include "Utilities/RandomNumberGenerator.h"
#include "Model/Objects.h"
#include "Estimates/Manager.h"


// namespaces
namespace niwa {
namespace timevarying {

/**
 * Default constructor
 */
RandomWalk::RandomWalk(Model* model) : TimeVarying(model) {
  parameters_.Bind<Double>(PARAM_MEAN, &mu_, "Mean", "", 0);
  parameters_.Bind<Double>(PARAM_SIGMA, &sigma_, "Standard deviation", "", 1);
  parameters_.Bind<Double>(PARAM_SIGMA, &rho_, "Auto Correlation parameter", "", 1);
  parameters_.Bind<string>(PARAM_DISTRIBUTION, &distribution_, "distribution", "", PARAM_NORMAL);

  RegisterAsAddressable(PARAM_MEAN, &mu_);
  RegisterAsAddressable(PARAM_SIGMA, &sigma_);
}

/**
 *
 */
void RandomWalk::DoValidate() {
  if (distribution_ != PARAM_NORMAL)
    LOG_ERROR() << "Random Walk can only can draw from a normal distribution currently";
}

/**
 *
 */
void RandomWalk::DoBuild() {
  Estimate* estimate = model_->managers().estimate()->GetEstimate(parameter_);
  if (estimate) {
    LOG_FINEST() << "Found an @estimate block for " << parameter_;
    has_at_estimate_ = true;
    upper_bound_ = estimate->upper_bound();
    lower_bound_ = estimate->lower_bound();
    if (model_->run_mode() == RunMode::kEstimation) {
      LOG_ERROR_P(PARAM_PARAMETER) << "You cannot have an @estimate block for a parameter that is time varying of type " << type_
          << ", casal2 will overwrite the estimate and a false minimum will be found";
    }
  }

  if(model_->objects().GetAddressableType(parameter_) != addressable::kSingle)
    LOG_ERROR_P(PARAM_TYPE) << "@time_varying blocks of type " << PARAM_RANDOMWALK << " can only be implemented in parameters that are scalars or single values";
}

/**
 *
 */
void RandomWalk::DoUpdate() {
  LOG_FINEST() << "value = " << AS_DOUBLE(*addressable_);
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();
  Double value = *addressable_;
  Double deviate = rng.normal(AS_DOUBLE(mu_), AS_DOUBLE(sigma_));
  value += value * rho_ + deviate;

  if (has_at_estimate_) {
    if (value < lower_bound_) {
      LOG_FINEST() << "hit @estimate lower bound setting value from " << value << " to " << lower_bound_;
      value = lower_bound_;
    }
    if (value > upper_bound_) {
      LOG_FINEST() << "hit @estimate upper bound setting value from " << value << " to " << upper_bound_;
      value = upper_bound_;
    }
  }
  if (value <= 0.0) {
    LOG_WARNING() << "parameter: " << parameter_ << " random draw of value = " << value << " a natural lower bound of 0.0 has been forced so resetting the value = 0.01";
    value  = 0.01;
  }

  LOG_FINEST() << "value after deviate of " << deviate << " = " << value << " for year " << model_->current_year();

  LOG_FINE() << "Setting Value to: " << value;
  parameter_by_year_[model_->current_year()] = value;
  (this->*update_function_)(value);
}

} /* namespace timevarying */
} /* namespace niwa */
