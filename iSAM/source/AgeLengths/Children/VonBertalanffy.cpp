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

VonBertalanffy::VonBertalanffy(Model* model) : AgeLength(model) {
  parameters_.Bind<Double>(PARAM_LINF, &linf_, "TBA", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_K, &k_, "TBA", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_T0, &t0_, "TBA", "");
  parameters_.Bind<string>(PARAM_LENGTH_WEIGHT, &length_weight_label_, "TBA", "");
  parameters_.Bind<Double>(PARAM_CV_FIRST, &cv_first_ , "CV for the first age class", "",Double(0.0))->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_CV_LAST, &cv_last_ , "CV for last age class", "",Double(0.0))->set_lower_bound(0.0);
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
} /* namespace agelengths */
} /* namespace niwa */
