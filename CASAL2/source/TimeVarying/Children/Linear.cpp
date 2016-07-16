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
  parameters_.Bind<Double>(PARAM_SLOPE, &slope_, "The slope of the linear trend (additive unit per year)", "", 0);

  RegisterAsEstimable(PARAM_SLOPE, &slope_);
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
  Estimate* estimate = model_->managers().estimate()->GetEstimate(parameter_);
  if (estimate) {
    LOG_FINEST() << "Found an @estimate block for " << parameter_;
    upper_bound_ = estimate->upper_bound();
    lower_bound_ = estimate->lower_bound();
  }

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
  LOG_FINE() << "current year " << model_->current_year();
  LOG_FINE() << "First year to implement " << years_[0];
  bool current_year =  model_->current_year() == years_[0];
  LOG_FINE() << " did we make it past this if statement " << current_year;
  if (current_year) {
    // First year don't make a change
    LOG_FINE() << "Setting Value to: " << (*value_);
    parameter_by_year_[model_->current_year()] = (*value_);
    (this->*update_function_)(*value_);
  } else {
    // Add a linear trend
    value_ = model_->objects().GetEstimable(parameter_, error);
    LOG_FINEST() << "value = " << (*value_);

    (*value_) += slope_;

    if ((*value_) < lower_bound_) {
      LOG_FINEST() << "hit @estimate lower bound setting value from "
          << (*value_) << " to " << lower_bound_;
      (*value_) = lower_bound_;
    }
    if ((*value_) > upper_bound_) {
      LOG_FINEST() << "hit @estimate upper bound setting value from "
          << (*value_) << " to " << upper_bound_;
      (*value_) = upper_bound_;
    }

    LOG_FINEST() << "value after deviate of " << slope_ << " = " << (*value_) << " for year " << model_->current_year();

    LOG_FINE() << "Setting Value to: " << (*value_);
    parameter_by_year_[model_->current_year()] = (*value_);
    (this->*update_function_)(*value_);
  }
}

} /* namespace timevarying */
} /* namespace niwa */
