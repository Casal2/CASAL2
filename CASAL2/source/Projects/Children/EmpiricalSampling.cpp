/**
 * @file EmpiricalSampling.cpp
 * @author  Craig Marsh
 * @date 05/02/2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 */

// headers
#include "EmpiricalSampling.h"

#include "Utilities/RandomNumberGenerator.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {
namespace projects {

/**
 * Default constructor
 */
EmpiricalSampling::EmpiricalSampling(Model* model) : Project(model) {
  parameters_.Bind<unsigned>(PARAM_START_YEAR, &start_year_, "Start year of sampling", "");
  parameters_.Bind<unsigned>(PARAM_FINAL_YEAR, &final_year_, "Last year of sampling", "");
}

/**
 * Validate
 */
void EmpiricalSampling::DoValidate() {
  if (final_year_ <= start_year_)
    LOG_ERROR() << PARAM_FINAL_YEAR << " must be larger than " << PARAM_START_YEAR;

}

/**
 * Build
 */
void EmpiricalSampling::DoBuild() {
  // Build a vector of years that have been resampled with replacement between start_year and end_year
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();

  for (unsigned project_year : years_) {
    Double Random_draw = round(rng.uniform(start_year_, final_year_));
    unsigned year = 0;
    if (!utilities::To<Double, unsigned>(Random_draw, year))
      LOG_ERROR() << " Random Draw " << Random_draw << " Could not be converted from double to type unsigned";
    resampled_years_[project_year] = year;
  }
}

/**
 * Reset
 */
void EmpiricalSampling::DoReset() { }

/**
 *  Update our parameter with a random resample of the parameter between start_year_ and final_year_
 */
void EmpiricalSampling::DoUpdate() {
  value_ = projected_parameters_[estimable_parameter_][resampled_years_[model_->current_year()]];
  LOG_FINE() << "In year: " << model_->current_year() << " Setting Value to: " << value_ << " drawn from year: " << resampled_years_[model_->current_year()];
  LOG_FINE() << "type of parameter = " << estimable_type_;
  (this->*DoUpdateFunc_)(value_);

}

} /* namespace projects */
} /* namespace niwa */
