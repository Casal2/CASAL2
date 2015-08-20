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
VonBertalanffy::VonBertalanffy() : VonBertalanffy(Model::Instance()) {
}

VonBertalanffy::VonBertalanffy(ModelPtr model) : AgeLength(model) {
  parameters_.Bind<Double>(PARAM_LINF, &linf_, "TBA", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_K, &k_, "TBA", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_T0, &t0_, "TBA", "");
  parameters_.Bind<string>(PARAM_LENGTH_WEIGHT, &length_weight_label_, "TBA", "");
  parameters_.Bind<Double>(PARAM_CV_FIRST, &cv_first_ , "CV for the first age class", "",Double(0.0))->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_CV_LAST, &cv_last_ , "CV for last age class", "",Double(0.0))->set_lower_bound(0.0);
  parameters_.Bind<string>(PARAM_DISTRIBUTION, &distribution_, "TBA", "", PARAM_NORMAL);
  parameters_.Bind<bool>(PARAM_BY_LENGTH, &by_length_, "Specifies if the linear interpolation of CV's is a linear function of mean length at age. Default is just by age", "", true);

  RegisterAsEstimable(PARAM_LINF, &linf_);
  RegisterAsEstimable(PARAM_K, &k_);
  RegisterAsEstimable(PARAM_T0, &t0_);
  RegisterAsEstimable(PARAM_CV_FIRST, &cv_first_);
  RegisterAsEstimable(PARAM_CV_LAST, &cv_last_);
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 */
void VonBertalanffy::DoBuild() {
  length_weight_ = model_->managers().length_weight().GetLengthWeight(length_weight_label_);
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
  Double proportion = time_step_proportions_[model_->managers().time_step().current_time_step()];
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
  Double size = this->mean_length(year, age);
  Double mean_weight = 0.0; //
  mean_weight = length_weight_->mean_weight(size, distribution_, cvs_[age]);// make a map [key = age]
  return mean_weight;
}

/*
 * Create look up vector of CV's that gets feed into mean_weight
 * And Age Length Key.
 * if cv_last_ and cv_first_ are time varying then this should be built every year
 * also if by_length_ is called, it will be time varying because it calls mean_weight which has time_varying
 * parameters. Otherwise it only needs to be built once a model run I believe
 */
void VonBertalanffy::BuildCV(unsigned year) {
  LOG_TRACE();
  unsigned min_age = model_->min_age();
  unsigned max_age = model_->max_age();

  // A test that is robust... If cv_last_ is not in the input then assume cv_first_ represents the cv for all age classes i.e constant cv
  if (cv_last_ == 0.0) {
    for (unsigned i = min_age; i <= max_age; ++i)
      cvs_[i] = cv_first_;

  } else if (by_length_) {  // if passed the first test we have a min and max CV. So ask if this is interpolated by length at age
    for (unsigned i = min_age; i <= max_age; ++i)
      cvs_[i] = ((mean_length(year, i) - mean_length(year, min_age)) * (cv_last_ - cv_first_) / (mean_length(year, max_age) - mean_length(year, min_age)) + cv_first_);

  } else {
    // else Do linear interpolation between cv_first_ and cv_last_ based on age class
    for (unsigned i = min_age; i <= max_age; ++i) {
      cvs_[i] = (cv_first_ + (cv_last_ - cv_first_) * (i - min_age) / (max_age - min_age));
    }
  }
}



void VonBertalanffy::CummulativeNormal(Double mu, Double cv, vector<Double>& prop_in_length, vector<Double> length_bins, string distribution, bool plus_grp) {
  // est proportion of age class that are in each length interval

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
    tmp = tmp - 0.356563782 * ttt;          // tt^2
    ttt = ttt * tt;
    tmp = tmp + 1.781477937 * ttt;       // tt^3
    ttt = ttt * tt;
    tmp = tmp - 1.821255978 * ttt;        // tt^4
    ttt = ttt * tt;
    tmp = tmp + 1.330274429 * ttt;      // tt^5

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
 */
void VonBertalanffy::DoAgeToLengthConversion(partition::Category* category, const vector<Double>& length_bins, bool plus_grp, SelectivityPtr selectivity) {
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
    }
  }
}

} /* namespace agelengths */
} /* namespace niwa */
