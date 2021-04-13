/**
 * @file AgeLength.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 23/02/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// defines
#define _USE_MATH_DEFINES

// headers
#include "AgeLength.h"

#include <cmath>

#include "../Model/Managers.h"
#include "../TimeSteps/Manager.h"
#include "../Utilities/Map.h"
#include "../Estimates/Manager.h"


// namespaces
namespace niwa {

/**
 * Default constructor
 *
 * Bind any parameters that are allowed to be loaded from the configuration files.
 * Set bounds on registered parameters
 * Register any parameters that can be an estimated or utilised in other run modes (e.g., profiling, yields, projections, etc.)
 * Set some initial values
 *
 * Note: The constructor is parsed to generate LaTeX for the documentation.
 */
AgeLength::AgeLength(shared_ptr<Model> model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "The label of the age length relationship", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "The type of age length relationship", "");
  parameters_.Bind<Double>(PARAM_TIME_STEP_PROPORTIONS, &time_step_proportions_, "The fraction of the year applied in each time step that is added to the age for the purposes of evaluating the length, i.e., a value of 0.5 for a time step will evaluate the length of individuals at age+0.5 in that time step", "", true)->set_range(0.0, 1.0);
  parameters_.Bind<string>(PARAM_DISTRIBUTION, &distribution_label_, "The assumed distribution for the growth curve", "", PARAM_NORMAL);
  parameters_.Bind<Double>(PARAM_CV_FIRST, &cv_first_ , "The CV for the first age class", "", Double(0.0))->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_CV_LAST, &cv_last_ , "The CV for last age class", "", Double(0.0))->set_lower_bound(0.0);
  parameters_.Bind<bool>(PARAM_CASAL_SWITCH, &casal_normal_cdf_ , "If true, use the (less accurate) equation for the cumulative normal function as was used in the legacy version of CASAL.", "",false);
  parameters_.Bind<bool>(PARAM_BY_LENGTH, &by_length_, "Specifies if the linear interpolation of CVs is a linear function of mean length at age. Default is by age only", "", true);

  RegisterAsAddressable(PARAM_CV_FIRST, &cv_first_);
  RegisterAsAddressable(PARAM_CV_LAST, &cv_last_);
}

/**
 * Populate any parameters,
 * Validate that values are within expected ranges when bind<>() overloads cannot be used
 *
 * Note: all parameters are populated from configuration files
 */
void AgeLength::Validate() {
  LOG_FINEST() << "by_length_ = " << by_length_;
  parameters_.Populate(model_);

  if (distribution_label_ == PARAM_NORMAL)
    distribution_ = Distribution::kNormal;
  else if (distribution_label_ == PARAM_LOGNORMAL)
    distribution_ = Distribution::kLogNormal;
  else if (distribution_label_ == PARAM_NONE)
    distribution_ = Distribution::kNone;
  else
    LOG_CODE_ERROR() << "The age-length distribution '" << distribution_label_ << "' is not valid.";

  DoValidate();
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 */
void AgeLength::Build() {
  unsigned time_step_count = model_->managers()->time_step()->ordered_time_steps().size();
  if (time_step_proportions_.size() == 0) {
    time_step_proportions_.assign(time_step_count, 0.0);
  } else if (time_step_count != time_step_proportions_.size()) {
    LOG_FATAL_P(PARAM_TIME_STEP_PROPORTIONS) << "size (" << time_step_proportions_.size() << ") must match the number "
      << "of defined time steps for this process (" << time_step_count << ")";
  }

  for (auto iter : time_step_proportions_) {
    if (iter < 0.0 || iter > 1.0)
      LOG_ERROR_P(PARAM_TIME_STEP_PROPORTIONS) << "Time step proportion value (" << iter << ") must be in the range 0.0 to 1.0 inclusive";
  }

  DoBuild();
  BuildCV();
}

/**
 * Calculate the CVs
 * populates a 3-D map of CVs by year, age, and time_step
 */
void AgeLength::BuildCV() {
  unsigned min_age = model_->min_age();
  unsigned max_age = model_->max_age();

  vector<unsigned> years(model_->years().size() + 1,0);
  unsigned year_ndx = 1;
  for (unsigned year : model_->years()) {
    years[year_ndx] = year;
    ++year_ndx;
  }
  vector<string> time_steps = model_->time_steps();

  for (unsigned year : years) {
    for (unsigned step_iter = 0; step_iter < time_steps.size(); ++step_iter) {
      if (!parameters_.Get(PARAM_CV_LAST)->has_been_defined()) { // TODO: Fix this #this test is robust but not compatible with testing framework, blah
        //If cv_last_ is not defined in the input then assume cv_first_ represents the cv for all age classes i.e constant cv
        for (unsigned age = min_age; age <= max_age; ++age)
          cvs_[year][step_iter][age] = (cv_first_);

      } else if (by_length_) {  // if passed the first test we have a min and max CV. So ask if this is linear interpolated by length at age
        for (unsigned age = min_age; age <= max_age; ++age) {
          cvs_[year][step_iter][age] = ((this->mean_length(step_iter, age) - this->mean_length(step_iter, min_age)) * (cv_last_ - cv_first_)
              / (this->mean_length(step_iter, max_age) - this->mean_length(step_iter, min_age)) + cv_first_);
        }
      } else {
        // else Do linear interpolation between cv_first_ and cv_last_ based on age class
        for (unsigned age = min_age; age <= max_age; ++age)
          cvs_[year][step_iter][age] = (cv_first_ + (cv_last_ - cv_first_) * (age - min_age) / (max_age - min_age));

      } // if (!parameters_.Get(PARAM_CV_LAST)->has_been_defined()) {
    } // for (unsigned step_iter = 0; step_iter < time_steps.size(); ++step_iter)
  } // for (unsigned year : years) {
}

/**
 * Reset the age length class
 */
void AgeLength::Reset() {
  if (is_estimated_) {
    LOG_FINEST() << "Rebuilding cv lookup table";
    BuildCV();
  }

  LOG_FINE() << "Resetting age-length";
  DoReset();
}

/**
 * ReBuild Cache: called by the time_varying class
 */
void AgeLength::RebuildCache() {
  LOG_FINE() << "Rebuilding age-length cache for year " << model_->current_year() << " run mode " << model_->run_mode()
    << " state " << model_->state();

  BuildCV();
  DoRebuildCache();
}

} /* namespace niwa */
