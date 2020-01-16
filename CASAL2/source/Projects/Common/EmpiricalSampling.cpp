/**
 * @file EmpiricalSampling.cpp
 * @author  Craig Marsh
 * @date 05/02/2016
 * @section LICENSE
 *
 * Copyright NIWA Science �2016 - www.niwa.co.nz
 *
 */

// headers
#include <Projects/Common/EmpiricalSampling.h>
#include "Utilities/RandomNumberGenerator.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {
namespace projects {

/**
 * Default constructor
 */
EmpiricalSampling::EmpiricalSampling(Model* model) : Project(model) {
  parameters_.Bind<unsigned>(PARAM_START_YEAR, &start_year_, "Start year of sampling", "", false);
  parameters_.Bind<unsigned>(PARAM_FINAL_YEAR, &final_year_, "Final year of sampling", "", false);
}

/**
 * Validate
 */
void EmpiricalSampling::DoValidate() {
  // if no values specified then set default as the model lifespan
  if (!parameters_.Get(PARAM_START_YEAR)->has_been_defined())
    start_year_ = model_->start_year();
  if (!parameters_.Get(PARAM_FINAL_YEAR)->has_been_defined())
    final_year_ = model_->final_year();

  if (final_year_ <= start_year_)
    LOG_ERROR_P(PARAM_FINAL_YEAR) << " " << final_year_ << " must be larger than start year " << start_year_;

}

/**
 * Build
 */
void EmpiricalSampling::DoBuild() {

}

/**
 * Reset
 */
void EmpiricalSampling::DoReset() {
  // Build a vector of years that have been resampled with replacement between start_year and end_year
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();
  Double Random_draw = 0.0;
  unsigned year = 0;
  for (unsigned project_year : years_) {
    Random_draw = ceil(rng.uniform((unsigned)start_year_, (unsigned)final_year_));
    year = 0;
    // if (!utilities::To<Double, unsigned>(Random_draw, year))
    if (!utilities::To<Double>(Random_draw, year))
      LOG_ERROR() << " Random Draw " << Random_draw << " could not be converted to Double";
    resampled_years_[project_year] = year;
    LOG_FINEST() << "Value from year: " << year << " used in projection year: " << project_year;
  }
}

/**
 *  Update our parameter with a random resample of the parameter between start_year_ and final_year_
 */
void EmpiricalSampling::DoUpdate() {
  value_ = stored_values_[resampled_years_[model_->current_year()]] * multiplier_;
  LOG_FINE() << "In year: " << model_->current_year() << " Setting Value to: " << value_ << " drawn from year: "
    << resampled_years_[model_->current_year()];
  (this->*DoUpdateFunc_)(value_);
}

} /* namespace projects */
} /* namespace niwa */
