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

#include "Model/Managers.h"
#include "TimeSteps/Manager.h"
#include "Utilities/Map.h"

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
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type", "");
  parameters_.Bind<Double>(PARAM_TIME_STEP_PROPORTIONS, &time_step_proportions_, "", "", true);
  parameters_.Bind<string>(PARAM_DISTRIBUTION, &distribution_, "TBA", "", PARAM_NORMAL);
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when we cannot use bind<>() overloads
 *
 * Note: all parameters are populated from configuration files
 */
void AgeLength::Validate() {
  parameters_.Populate();

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
}

/**
 * BuildCV function
 *
 * @param year currentyear
 * @return vector<Double> CV for each age
 */
void AgeLength::BuildCV(unsigned year) {
  unsigned min_age = model_->min_age();
  unsigned max_age = model_->max_age();

  if (cv_last_==0) { // A test that is robust... If cv_last_ is not in the input then assume cv_first_ represents the cv for all age classes i.e constant cv
    for (unsigned i = min_age; i <= max_age; ++i)
      cvs_[i]= (cv_first_);
    } else {
    // else Do linear interpolation between cv_first_ and cv_last_ based on age class
    for (unsigned i = min_age; i <= max_age; ++i) {
      cvs_[i]= (cv_first_ + (cv_last_ - cv_first_) * (i - min_age) / (max_age - min_age));
    }
  }

}

/*
  * Calculates the proportion in each length_bins for a single age
  *
  *@param mu mean of length bin
  *@param mu mean length for an age
  *@param cv cv of the age
  *@param prop_in_length reference parameter that proportions are stored in
  *@param length_bins a vector of minimum values for each length bin
  *@param distribution use the Normal or Lognormal distribution
  *@param plus_grp whether the last length bin is a plus group
  *
 */
void AgeLength::CummulativeNormal(Double mu, Double cv, vector<Double>& prop_in_length, vector<Double> length_bins, string distribution, bool plus_grp) {

  Double sigma = cv * mu;

  if (distribution == "lognormal") {
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
  vector<Double> cum;

  std::vector<int>::size_type sz = length_bins.size();
  prop_in_length.resize(sz);

  for (unsigned j = 0; j < sz; ++j) {
    z = fabs((length_bins[j] - mu)) / sigma;

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

    cum.push_back(1.0 - norm * tmp);
    if (length_bins[j] < mu) {
      cum[j] = 1.0 - cum[j];
    }
    if (j > 0) {
      prop_in_length[j - 1] = cum[j] - cum[j - 1];
      sum += prop_in_length[j - 1];
    }
  }
  if (plus_grp) {
    prop_in_length[sz - 1] = 1.0 - sum - cum[0];
  } else
    prop_in_length.resize(sz - 1);
}

/**
 * Do the conversion of the partition structure from age to length
 *
 * @param category The current category to convert
 * @param length_bins vector of the length bins to map too
 * @param selectivity SelectivityPointer to apply (age based selectivity)
 */
void AgeLength::DoAgeToLengthConversion(partition::Category* category, const vector<Double>& length_bins, bool plus_grp, Selectivity* selectivity) {
  LOG_TRACE();
  unsigned size = length_bins.size();
  if (!plus_grp)
    size = length_bins.size() - 1;

  category->age_length_matrix_.resize(category->data_.size());
  for (unsigned i = 0; i < category->data_.size(); ++i) {
    vector<Double> age_frequencies;
    unsigned age = category->min_age_ + i;

    Double mu= category->mean_length_per_[age];
    CummulativeNormal(mu, cvs_[age], age_frequencies, length_bins, distribution_, plus_grp);
    category->age_length_matrix_[i].resize(size);

    // Loop through the length bins and multiple the partition of the current age to go from
    // length frequencies to age length numbers
    for (unsigned j = 0; j < size; ++j) {
      category->age_length_matrix_[i][j] = selectivity->GetResult(age) * category->data_[i] * age_frequencies[j];
 //     LOG_FINEST() << " Selectivity for age: " << age << " = " << selectivity->GetResult(age) << " With numbers at age = " << category->data_[i] << " probability for bin " << length_bins[j] << " = " <<  age_frequencies[j];
 //     LOG_FINEST() << " When all the above multiplied we get " << category->age_length_matrix_[i][j];
    }
  }
}


} /* namespace niwa */
