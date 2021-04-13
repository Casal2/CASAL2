/**
 * @file LogNormal.cpp
 * @author  Craig Marsh
 * @date 05/02/2016
 * @section LICENSE
 *
 * Copyright NIWA Science �2016 - www.niwa.co.nz
 *
 */

// headers
#include "LogNormal.h"
#include "../../Utilities/RandomNumberGenerator.h"
#include "../../Model/Objects.h"

// namespaces
namespace niwa {
namespace projects {

/**
 * Default constructor
 */

LogNormal::LogNormal(shared_ptr<Model> model) : Project(model) {
  parameters_.Bind<Double>(PARAM_MEAN, &mean_, "The mean of the lognormal process", "", 0.0);
  parameters_.Bind<Double>(PARAM_SIGMA, &sigma_, "The standard deviation (sigma) of the lognormal process", "")->set_lower_bound(0.0);
  //parameters_.Bind<Double>(PARAM_RHO, &rho_, "an autocorrelation parameter on the log scale", "", 0.0);
  //parameters_.Bind<Double>(PARAM_ALPHA, &alpha_, "The alpha constant in the gaussian AR(1) process", 0.0,true);

}

/**
 * Validate
 */
void LogNormal::DoValidate() {

}

/**
 * Build
 */

void LogNormal::DoBuild() {
}

/**
 * Reset
 */
void LogNormal::DoReset() {
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();
  for (unsigned project_year : years_) {
    //if (parameters_.Get(PARAM_RHO)->has_been_defined()) {
    //   lognormal_draw_by_year_[project_year] = rng.normal(0.0, 1.0);
    //} else {
      normal_draw_by_year_[project_year] = rng.normal(AS_DOUBLE(mean_), AS_DOUBLE(sigma_));
    //}
    LOG_FINEST() << "generated value = " << normal_draw_by_year_[project_year] << " to be applied in year " << project_year;
  }
}

/**
 *  Update the parameter with a random draw from a lognormal distribution with specified inputs
 */
void LogNormal::DoUpdate() {
  // instance the random number generator
  //if (parameters_.Get(PARAM_RHO)->has_been_defined()) {
  //  LOG_WARNING() << "Calculating an AR(1) process for projections.";
  //  Double last_value_ = projected_parameters_[model_->current_year() - 1];
  //  Double Z = lognormal_draw_by_year_[model_->current_year()];
  //  value_ =exp((alpha_ + rho_ * last_value_ + Z) - 0.5 * sigma_ * sigma_);

  //} else {
    // Just a standard normal deviation
    value_ = exp(normal_draw_by_year_[model_->current_year()] - 0.5 * sigma_ * sigma_);
  // }
  // Store this value to be pulled out next projection year
  value_ = value_ * multiplier_;

  LOG_FINE() << "Setting Value to: " << value_;
  (this->*DoUpdateFunc_)(value_);
}

} /* namespace projects */
} /* namespace niwa */
