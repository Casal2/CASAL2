/**
 * @file RandomDraw.cpp
 * @author Craig Marsh
 * @github https://github.com/Zaita
 * @date 2/02/2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "RandomDraw.h"

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
RandomDraw::RandomDraw(Model* model) : TimeVarying(model) {
  parameters_.Bind<Double>(PARAM_MEAN, &mu_, "Mean", "", 0);
  parameters_.Bind<Double>(PARAM_SIGMA, &sigma_, "Standard deviation", "", 1);
  parameters_.Bind<string>(PARAM_DISTRIBUTION, &distribution_, "distribution", "", PARAM_NORMAL)->set_allowed_values({PARAM_NORMAL,PARAM_LOGNORMAL});

  RegisterAsAddressable(PARAM_MEAN, &mu_);
  RegisterAsAddressable(PARAM_SIGMA, &sigma_);
}

/**
 *
 */
void RandomDraw::DoValidate() {

}

/**
 *
 */
void RandomDraw::DoBuild() {
  Estimate* estimate = model_->managers().estimate()->GetEstimate(parameter_);
  if (estimate) {
    has_at_estimate_ = true;
    LOG_FINEST() << "Found an @estimate block for " << parameter_;
    upper_bound_ = estimate->upper_bound();
    lower_bound_ = estimate->lower_bound();
    if (model_->run_mode() == RunMode::kEstimation) {
      LOG_ERROR_P(PARAM_PARAMETER) << "You cannot have an @estimate block for a parameter that is time varying of type " << type_
          << ", casal2 will overwrite the estimate and a false minimum will be found";
    }
  }

  if(model_->objects().GetAddressableType(parameter_) != addressable::kSingle)
    LOG_ERROR_P(PARAM_TYPE) << "@time_varying blocks of type " << PARAM_RANDOMWALK << " can only be implemented in parameters that are scalars or single values";
  DoReset();
}

/**
 *
 */
void RandomDraw::DoReset() {
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();
  Double new_value = 0.0;
  // Draw from the random distribution
  if (distribution_ == PARAM_NORMAL) {
    for (unsigned year : years_) {
    new_value = rng.normal(AS_DOUBLE(mu_), AS_DOUBLE(sigma_));
    LOG_FINEST() << "with mean = " << mu_ << " and sigma = " << sigma_ << " new value = " << new_value;
    // Set value
    if (has_at_estimate_) {
      if (new_value < lower_bound_) {
        LOG_FINEST() << "hit @estimate lower bound setting value from " << new_value << " to " << lower_bound_;
        new_value = lower_bound_;
      }
      if (new_value > upper_bound_) {
        LOG_FINEST() << "hit @estimate upper bound setting value from " << new_value << " to " << upper_bound_;
        new_value = upper_bound_;
      }
    }
    if (new_value <= 0.0) {
      LOG_WARNING() << "parameter: " << parameter_ << " random draw of value = " << new_value << " a natural lower bound of 0.0 has been forced so resetting the value = 0.01";
      new_value  = 0.01;
    }
    parameter_by_year_[year] = new_value;
    }
  } else if (distribution_ == PARAM_LOGNORMAL)  {
    for (unsigned year : years_) {
      Double cv = sigma_ / mu_;
      new_value = rng.lognormal(AS_DOUBLE(mu_), AS_DOUBLE(cv));
      LOG_FINEST() << "with mean = " << mu_ << " and sigma = " << sigma_ << " new value = " << new_value;
      // Set value
      if (has_at_estimate_) {
        if (new_value < lower_bound_) {
          LOG_FINEST() << "hit @estimate lower bound setting value from " << new_value << " to " << lower_bound_;
          new_value = lower_bound_;
        }
        if (new_value > upper_bound_) {
          LOG_FINEST() << "hit @estimate upper bound setting value from " << new_value << " to " << upper_bound_;
          new_value = upper_bound_;
        }
      }
      if (new_value <= 0.0) {
        LOG_WARNING() << "parameter: " << parameter_ << " random draw of value = " << new_value << " a natural lower bound of 0.0 has been forced so resetting the value = 0.01";
        new_value  = 0.01;
      }
      parameter_by_year_[year] = new_value;
    }
  }
}

/**
 *
 */
void RandomDraw::DoUpdate() {
  LOG_FINE() << "Setting Value to: " << parameter_by_year_[model_->current_year()];
  (this->*update_function_)(parameter_by_year_[model_->current_year()]);
}

} /* namespace timevarying */
} /* namespace niwa */
