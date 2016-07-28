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
#include "Linear.h"

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

  RegisterAsEstimable(PARAM_SLOPE, &slope_);
  RegisterAsEstimable(PARAM_INTERCEPT, &intercept_);

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

}

/**
 *
 */
void Linear::DoReset() {

}

/**
 *
 */
void Linear::DoUpdate() {
  string error = "";
  value_ = model_->objects().GetEstimable(parameter_, error);
  if (value_ == nullptr)
    LOG_ERROR() << "The parameter you have chosen is not an estimable parameter";
  LOG_FINE() << "current year " << model_->current_year();
  LOG_FINE() << "First year to implement " << years_[0];
  bool current_year =  model_->current_year() == years_[0];

  unsigned diff = model_->current_year() - years_[0];
  LOG_FINE() << "diff unsigned = " << diff;

  Double years_since_first_year = (Double)model_->current_year() - (Double)years_[0];
  LOG_FINE() << "diff from start of year = " << years_since_first_year;
  LOG_FINE() << " did we make it past this if statement " << current_year;
  if (current_year) {
    // First year don't make a change
    LOG_FINE() << "Setting Value to: " << intercept_;
    (*value_) = intercept_;
    parameter_by_year_[model_->current_year()] = intercept_;
    (this->*update_function_)(*value_);
  } else {
    // Add a linear trend
    (*value_) = intercept_ + years_since_first_year * slope_;
    //value_ = model_->objects().GetEstimable(parameter_, error);
    LOG_FINE() << "value = " << (*value_);

    LOG_FINE() << "value after deviate of " << slope_ << " = " << (*value_) << " for year " << model_->current_year();

    LOG_FINE() << "Setting Value to: " << (*value_);
    parameter_by_year_[model_->current_year()] = (*value_);
    (this->*update_function_)(*value_);
  }
}

} /* namespace timevarying */
} /* namespace niwa */
