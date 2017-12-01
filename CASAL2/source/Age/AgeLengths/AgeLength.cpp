/**
 * @file AgeLength.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 23/02/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */

// defines
#define _USE_MATH_DEFINES

// headers
#include "AgeLength.h"

#include <cmath>

#include "Common/Model/Managers.h"
#include "Common/TimeSteps/Manager.h"
#include "Common/Utilities/Map.h"
#include "Common/Estimates/Manager.h"


// namespaces
namespace niwa {

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
AgeLength::AgeLength(Model* model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label of the age length relationship", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type of age length relationship", "");
  parameters_.Bind<Double>(PARAM_TIME_STEP_PROPORTIONS, &time_step_proportions_, "the fraction of the year applied in each time step that is added to the age for the purposes of evaluating the length, i.e., a value of 0.5 for a time step will evaluate the length of individuals at age+0.5 in that time step", "", true);
  parameters_.Bind<string>(PARAM_DISTRIBUTION, &distribution_label_, "The assumed distribution for the growth curve", "", PARAM_NORMAL);
  parameters_.Bind<Double>(PARAM_CV_FIRST, &cv_first_ , "CV for the first age class", "",Double(0.0))->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_CV_LAST, &cv_last_ , "CV for last age class", "",Double(0.0))->set_lower_bound(0.0);
  parameters_.Bind<bool>(PARAM_CASAL_SWITCH, &casal_normal_cdf_ , "If true, use the (less accurate) equation for the cumulative normal function as was used in the legacy version of CASAL.", "",false);
  parameters_.Bind<bool>(PARAM_BY_LENGTH, &by_length_, "Specifies if the linear interpolation of CV's is a linear function of mean length at age. Default is just by age", "", true);
  parameters_.Bind<Double>(PARAM_LENGTH_BINS, &length_bins_, "List of length bins for this age length", "", true);

  RegisterAsAddressable(PARAM_CV_FIRST, &cv_first_);
  RegisterAsAddressable(PARAM_CV_LAST, &cv_last_);
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when we cannot use bind<>() overloads
 *
 * Note: all parameters are populated from configuration files
 */
void AgeLength::Validate() {
  parameters_.Populate(model_);

  if (distribution_label_ == PARAM_NORMAL)
    distribution_ = Distribution::kNormal;
  else if (distribution_label_ == PARAM_LOGNORMAL)
    distribution_ = Distribution::kLogNormal;
  else
    LOG_CODE_ERROR() << "We haven't enum'd the distribution: " << distribution_label_;

  DoValidate();
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 */
void AgeLength::Build() {
  unsigned time_step_count = model_->managers().time_step()->ordered_time_steps().size();
  if (time_step_proportions_.size() == 0) {
    time_step_proportions_.assign(time_step_count, 0.0);
  } else if (time_step_count != time_step_proportions_.size()) {
    LOG_ERROR_P(PARAM_TIME_STEP_PROPORTIONS) << "size (" << time_step_proportions_.size() << ") must match the number "
        "of defined time steps for this process (" << time_step_count << ")";
  }

  for (auto iter : time_step_proportions_) {
    if (iter < 0.0 || iter > 1.0)
      LOG_ERROR_P(PARAM_TIME_STEP_PROPORTIONS) << " value (" << iter << ") must be in the range 0.0-1.0";
  }

  DoBuild();
  BuildCV();
}

/**
 * BuildCV function
 * populates a 3d map of cv's by year, age and time_step
 */
void AgeLength::BuildCV() {
  unsigned min_age = model_->min_age();
  unsigned max_age = model_->max_age();
  vector<unsigned> years = model_->years();
  vector<string> time_steps = model_->time_steps();
  LOG_FINE() << "number of time steps " << time_steps.size();

//  for (unsigned year_iter = 0; year_iter < years.size(); ++year_iter) {
  for (unsigned year_iter : years) {
    for (unsigned step_iter = 0; step_iter < time_steps.size(); ++step_iter) {
      if (!parameters_.Get(PARAM_CV_LAST)->has_been_defined()) { // this test is robust but not compatible with testing framework, blah
        //If cv_last_ is not defined in the input then assume cv_first_ represents the cv for all age classes i.e constant cv
        for (unsigned age_iter = min_age; age_iter <= max_age; ++age_iter) {
          cvs_[year_iter][step_iter][age_iter] = (cv_first_);
          //LOG_FINE() << "cv for age = " << age_iter << " in time_step " << step_iter << " in year " << year_iter << " = " << cvs_[year_iter][age_iter][step_iter];
        }
      } else if (by_length_) {  // if passed the first test we have a min and max CV. So ask if this is linear interpolated by length at age
        for (unsigned age_iter = min_age; age_iter <= max_age; ++age_iter)
          cvs_[year_iter][step_iter][age_iter] = ((this->mean_length(step_iter, age_iter) - this->mean_length(step_iter, min_age)) * (cv_last_ - cv_first_)
              / (this->mean_length(step_iter, max_age) - this->mean_length(step_iter, min_age)) + cv_first_);
      } else {
        // else Do linear interpolation between cv_first_ and cv_last_ based on age class
        for (unsigned age_iter = min_age; age_iter <= max_age; ++age_iter) {
          cvs_[year_iter][step_iter][age_iter] = (cv_first_ + (cv_last_ - cv_first_) * (age_iter - min_age) / (max_age - min_age));
        }
      }
    }
  }
}

/*
  * Calculates the proportion in each length_bins for a single age
  *
  *@param mu mean length for an age
  *@param cv cv of the age
  *@param prop_in_length reference parameter that proportions are stored in
  *@param length_bins a vector of minimum values for each length bin
  *@param plus_grp whether the last length bin is a plus group
  *
 */
void AgeLength::CummulativeNormal(Double mu, Double cv, vector<Double>& prop_in_length, vector<Double> length_bins, bool plus_grp) {

  Double sigma = cv * mu;
  if (distribution_ == Distribution::kLogNormal) {
    // Transform parameters in to log space
    Double cv_temp = sigma / mu;
    Double Lvar = log(cv_temp * cv_temp + 1.0);
    mu = log(mu) - Lvar / 2.0;
    sigma = sqrt(Lvar);

    for (Double& value : length_bins) {
      if (value < 0.0001)
        value = log(0.0001);
      else
        value = log(value);
    }
  }

  Double z, tt, norm, ttt, tmp;
  Double sum = 0;

  std::vector<int>::size_type sz = length_bins.size();
//  prop_in_length.resize(sz);
  vector<Double> cum(sz, 0.0);

  // TODO CAN BE THREADED - OPENMP
  for (unsigned j = 0; j < sz; ++j) {
    z = fabs((length_bins[j] - mu)) / sigma;

    // If we are using CASAL's Normal CDF function use this switch
    if (casal_normal_cdf_) {
      tmp = 0.5 * pow((1 + 0.196854 * z + 0.115194 * z * z + 0.000344 * z * z * z + 0.019527 * z * z * z * z), -4);
    } else {
      tt = 1.0 / (1.0 + 0.2316419 * z);
      norm = 1.0 / sqrt(2.0 * M_PI) * exp(-0.5 * z * z);
      ttt = tt;
      tmp = 0.319381530 * ttt;
      ttt = ttt * tt;
      tmp = tmp - 0.356563782 * ttt;
      ttt = ttt * tt;
      tmp = tmp + 1.781477937 * ttt;
      ttt = ttt * tt;
      tmp = tmp - 1.821255978 * ttt;
      ttt = ttt * tt;
      tmp = tmp + 1.330274429 * ttt;
      tmp *= norm;
    }
    tmp = 1.0 - tmp;
    cum[j] = tmp;
    if (length_bins[j] < mu) {
      cum[j] = 1.0 - cum[j];
    }
    if (j > 0) {
      prop_in_length[j - 1] = cum[j] - cum[j - 1];
      sum += prop_in_length[j - 1];
    }
  }
  if (plus_grp)
    prop_in_length[sz - 1] = 1.0 - sum - cum[0];
}

/**
 * Do the conversion of the partition structure from age to length
 *
 * @param category The current category to convert
 * @param length_bins vector of the length bins to map too
 * @param selectivity SelectivityPointer to apply (age based selectivity)
 */
void AgeLength::DoAgeToLengthMatrixConversion(partition::Category* category, const vector<Double>& length_bins, bool plus_grp, Selectivity* selectivity) {
  LOG_TRACE();
  unsigned year         = model_->current_year();
  unsigned size         = plus_grp == true ? length_bins.size() : length_bins.size() - 1;
  unsigned time_step    = model_->managers().time_step()->current_time_step();
  unsigned current_age  = 0;
  Double mu             = 0.0;

  vector<Double> ages_at_length_(size, 0.0);
  category->age_length_matrix_.resize(category->data_.size());

  for (unsigned i = 0; i < category->data_.size(); ++i) {
    category->age_length_matrix_[i].resize(size);

    current_age = category->min_age_ + i;
    if (cvs_[year][time_step][current_age] <= 0.0)
        LOG_FATAL_P(PARAM_CV_FIRST) << "Identified a CV of 0.0, in year " << year << ", for age " << current_age << " and time step = " << time_step << " please check parameters cv_first and cv_last in the @age_length, or make sure you have specified suitable bounds in the @estimate block";

    mu = category->mean_length_by_time_step_age_[time_step][current_age];
    CummulativeNormal(mu, cvs_[year][time_step][current_age], ages_at_length_, length_bins, plus_grp);
    category->age_length_matrix_[i].resize(size);

    // Loop through the length bins and multiple the partition of the current age to go from
    // length frequencies to age length numbers
    for (unsigned j = 0; j < size; ++j) {
      category->age_length_matrix_[i][j] = selectivity->GetAgeResult(current_age, category->age_length_) * category->data_[i] * ages_at_length_[j];
    }
  }
}

/**
 * Reset the age length class.
 */
void AgeLength::Reset() {
  if (is_estimated_) {
    LOG_FINEST() << "We are re-building cv lookup table.";
    BuildCV();
  }
  DoReset();
}

/**
 * ReBuild Cache: intiated by the time_varying class.
 */
void AgeLength::RebuildCache() {
  BuildCV();
  DoRebuildCache();
}

} /* namespace niwa */
