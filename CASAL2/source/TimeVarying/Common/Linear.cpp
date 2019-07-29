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
#include <TimeVarying/Common/Linear.h>
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
Linear::Linear(Model* model) : TimeVarying(model) {
  parameters_.Bind<Double>(PARAM_SLOPE, &slope_, "The slope of the linear trend (additive unit per year)", "");
  parameters_.Bind<Double>(PARAM_INTERCEPT, &intercept_, "The intercept of the linear trend value for the first year", "");

  RegisterAsAddressable(PARAM_SLOPE, &slope_);
  RegisterAsAddressable(PARAM_INTERCEPT, &intercept_);

}

/**
 *
 */
void Linear::DoValidate() {

}

/**
 *
 */
void Linear::DoBuild() {
  if(model_->objects().GetAddressableType(parameter_) != addressable::kSingle)
    LOG_ERROR_P(PARAM_TYPE) << "@time_varying blocks of type " << PARAM_LINEAR << " can only be implemented in parameters that are scalars or single values";
  DoReset();
}

/**
 *
 */
void Linear::DoReset() {
  bool current_year =  model_->current_year() == years_[0];
  unsigned diff = model_->current_year() - years_[0];
  LOG_FINE() << "diff unsigned = " << diff;

  Double years_since_first_year = (Double)model_->current_year() - (Double)years_[0];
  LOG_FINE() << "diff from start of year = " << years_since_first_year;
  LOG_FINE() << " did we make it past this if statement " << current_year;
  if (current_year) {
    // First year don't make a change
    LOG_FINE() << "Setting Value to: " << intercept_;
    parameter_by_year_[model_->current_year()] =  intercept_;
  } else {
    // Add a linear trend
    parameter_by_year_[model_->current_year()] = intercept_ + years_since_first_year * slope_;
    //value_ = model_->objects().GetEstimable(parameter_, error);
    LOG_FINE() << "value = " << parameter_by_year_[model_->current_year()];
    LOG_FINE() << "value after deviate of " << slope_ << " = " << parameter_by_year_[model_->current_year()] << " for year " << model_->current_year();
    LOG_FINE() << "Setting Value to: " << parameter_by_year_[model_->current_year()];
  }
}

/**
 *
 */
void Linear::DoUpdate() {
    (this->*update_function_)(parameter_by_year_[model_->current_year()]);
}

} /* namespace timevarying */
} /* namespace niwa */
