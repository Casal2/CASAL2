/**
 * @file VonBertalanffy.cpp

 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 14/08/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// defines
#define _USE_MATH_DEFINES

// headers
#include "VonBertalanffy.h"

#include <cmath>

#include "Model/Managers.h"
#include "LengthWeights/LengthWeight.h"
#include "LengthWeights/Manager.h"
#include "TimeSteps/Manager.h"

// namespaces
namespace niwa {
namespace agelengths {

/**
 * Default constructor
 *
 * Bind any parameters that are allowed to be loaded from the configuration files.
 * Set bounds on registered parameters
 * Register any parameters that can be an estimated or utilised in other run modes (e.g profiling, yields, projections etc)
 * Set some initial values
 *
 * Note: The constructor is parsed to generate Latex for the documentation.
 */
VonBertalanffy::VonBertalanffy(Model* model) : AgeLength(model) {
  parameters_.Bind<Double>(PARAM_LINF, &linf_, "TBA", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_K, &k_, "TBA", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_T0, &t0_, "TBA", "");
  parameters_.Bind<string>(PARAM_LENGTH_WEIGHT, &length_weight_label_, "TBA", "");
  parameters_.Bind<bool>(PARAM_BY_LENGTH, &by_length_, "Specifies if the linear interpolation of CV's is a linear function of mean length at age. Default is just by age", "", true);

  RegisterAsEstimable(PARAM_LINF, &linf_);
  RegisterAsEstimable(PARAM_K, &k_);
  RegisterAsEstimable(PARAM_T0, &t0_);
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 */
void VonBertalanffy::DoBuild() {
  length_weight_ = model_->managers().length_weight()->GetLengthWeight(length_weight_label_);
  if (!length_weight_)
    LOG_ERROR_P(PARAM_LENGTH_WEIGHT) << "(" << length_weight_label_ << ") could not be found. Have you defined it?";
}

/**
 * Get the mean length of a single population
 *
 * @param year The year we want mean length for
 * @param age The age of the population we want mean length for
 * @return The mean length for 1 member
 */
Double VonBertalanffy::mean_length(unsigned year, unsigned age) {
  Double proportion = time_step_proportions_[model_->managers().time_step()->current_time_step()];
  if ((-k_ * ((age + proportion) - t0_)) > 10)
    LOG_ERROR_P(PARAM_K) << "exp(-k*(age-t0)) is enormous. The k or t0 parameters are probably wrong.";

  Double size = linf_ * (1 - exp(-k_ * ((age + proportion) - t0_)));
  if (size < 0.0)
    return 0.0;

  return size;
}

/**
 * Get the mean weight of a single population
 *
 * @param year The year we want mean weight for
 * @param age The age of the population we want mean weight for
 * @return mean weight for 1 member
 */
Double VonBertalanffy::mean_weight(unsigned year, unsigned age) {
  unsigned time_step = model_->managers().time_step()->current_time_step();
  Double size = this->mean_length(year, age);
  Double mean_weight = 0.0; //
  mean_weight = length_weight_->mean_weight(size, distribution_, cvs_[year][age][time_step]);// make a map [key = age]
  return mean_weight;
}

/*
 * Create a 2d look up map of CV's that gets used in mean_weight and any distribution around
 * converting age to length
 */
void VonBertalanffy::BuildCV() {
  LOG_TRACE();
  unsigned min_age = model_->min_age();
  unsigned max_age = model_->max_age();
  unsigned start_year = model_->start_year();
  unsigned final_year = model_->final_year();
  vector<string> time_steps = model_->time_steps();
  LOG_MEDIUM() << ": number of time steps " << time_steps.size();
  LOG_MEDIUM() << ": label of first time step " << time_steps[0];
  for (unsigned year_iter = start_year; year_iter <= final_year; ++year_iter) {
    for (unsigned step_iter = 0; step_iter < time_steps.size(); ++step_iter) {
      if (cv_last_ == 0.0) { // A test that is robust... If cv_last_ is not in the input then assume cv_first_ represents the cv for all age classes i.e constant cv
        for (unsigned age_iter = min_age; age_iter <= max_age; ++age_iter)
          cvs_[year_iter][age_iter][step_iter] = (cv_first_);

      } else if (by_length_) {  // if passed the first test we have a min and max CV. So ask if this is interpolated by length at age
        for (unsigned age_iter = min_age; age_iter <= max_age; ++age_iter)
          cvs_[year_iter][age_iter][step_iter] = ((mean_length(year_iter, age_iter) - mean_length(year_iter, min_age)) * (cv_last_ - cv_first_)
              / (mean_length(year_iter, max_age) - mean_length(year_iter, min_age)) + cv_first_);

      } else {
        // else Do linear interpolation between cv_first_ and cv_last_ based on age class
        for (unsigned age_iter = min_age; age_iter <= max_age; ++age_iter) {
          cvs_[year_iter][age_iter][step_iter] = (cv_first_ + (cv_last_ - cv_first_) * (age_iter - min_age) / (max_age - min_age));
        }
      }
    }
  }
}

} /* namespace agelengths */
} /* namespace niwa */
