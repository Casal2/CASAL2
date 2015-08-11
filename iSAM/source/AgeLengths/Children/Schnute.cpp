/**
 * @file Schnute.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 19/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "Schnute.h"

#include <cmath>

#include "Model/Managers.h"

// namespaces
namespace niwa {
namespace agelengths {

using std::pow;

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
Schnute::Schnute() : Schnute(Model::Instance()) {
}

Schnute::Schnute(ModelPtr model) : AgeLength(model) {
  parameters_.Bind<Double>(PARAM_Y1, &y1_, "TBA", "");
  parameters_.Bind<Double>(PARAM_Y2, &y2_, "TBA", "");
  parameters_.Bind<Double>(PARAM_TAU1, &tau1_, "TBA", "");
  parameters_.Bind<Double>(PARAM_TAU2, &tau2_, "TBA", "");
  parameters_.Bind<Double>(PARAM_A, &a_, "TBA", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_B, &b_, "TBA", "")->set_lower_bound(0.0, false);
  parameters_.Bind<string>(PARAM_LENGTH_WEIGHT, &length_weight_label_, "TBA", "");
  parameters_.Bind<Double>(PARAM_CV_FIRST, &cv_first_ , "CV for the first age class", "",Double(0.0))->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_CV_LAST, &cv_last_ , "CV for maximum age", "",Double(0.0))->set_lower_bound(0.0);
  parameters_.Bind<string>(PARAM_DISTRIBUTION, &distribution_, "TBA", "", PARAM_NORMAL);
  parameters_.Bind<bool>(PARAM_BY_LENGTH, &by_length_, "TBA", "", true);

  RegisterAsEstimable(PARAM_Y1, &y1_);
  RegisterAsEstimable(PARAM_Y2, &y2_);
  RegisterAsEstimable(PARAM_TAU1, &tau1_);
  RegisterAsEstimable(PARAM_TAU2, &tau2_);
  RegisterAsEstimable(PARAM_A, &a_);
  RegisterAsEstimable(PARAM_B, &b_);
  RegisterAsEstimable(PARAM_CV_FIRST, &cv_first_);
  RegisterAsEstimable(PARAM_CV_LAST, &cv_last_);
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 */
void Schnute::DoBuild() {
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
Double Schnute::mean_length(unsigned year, unsigned age) {
  Double temp = 0.0;
  Double size = 0.0;

  Double proportion = time_step_proportions_[model_->managers().time_step().current_time_step()];

  if (a_ != 0.0)
    temp = (1 - exp( -a_ * ((age + proportion) - tau1_))) / (1 - exp(-a_ * (tau2_ - tau1_)));
  else
    temp = (age - tau1_) / (tau2_ - tau1_);

  if (b_ != 0.0)
    size = pow((pow(y1_, b_) + (pow(y2_, b_) - pow(y1_, b_)) * temp), 1 / b_);
  else
    size = y1_ * exp(log(y2_ / y1_) * temp);

  if (size < 0.0)
    return 0.0;

  return size;
}

/**
 * Create look up vector of CV's that gets feed into mean_weight
 * And Age Length Key.
 * if cv_last_ and cv_first_ are time varying then this should be built every year
 * also if by_length_ is called, it will be time varying because it calls mean_weight which has time_varying
 * parameters. Otherwise it only needs to be built once a model run I believe
 */
void Schnute::BuildCV(unsigned year) {
  unsigned min_age = model_->min_age();
  unsigned max_age = model_->max_age();

  if (cv_last_==0) { // A test that is robust... If cv_last_ is not in the input then assume cv_first_ represents the cv for all age classes i.e constant cv
    for (unsigned i = min_age; i <= max_age; ++i) {
      cvs_[i]= (cv_first_);
    }
  } else if(by_length_) {  // if passed the first test we have a min and max CV. So ask if this is interpolated by length at age
    for (unsigned i = min_age; i <= max_age; ++i) {
      cvs_[i]= ((mean_length(year, i) - mean_length(year, min_age)) * (cv_last_ - cv_first_) / (mean_length(year, max_age) - mean_length(year, min_age)) + cv_first_);
    }

  } else {
    // else Do linear interpolation between cv_first_ and cv_last_ based on age class
    for (unsigned i = min_age; i <= max_age; ++i) {
      cvs_[i]= (cv_first_ + (cv_last_ - cv_first_) * (i - min_age) / (max_age - min_age));
    }
  }

}


/**
 * Get the mean weight of a single population
 *
 * @param year The year we want mean weight for
 * @param age The age of the population we want mean weight for
 * @return mean weight for 1 member
 */
Double Schnute::mean_weight(unsigned year, unsigned age) {
  Double size = this->mean_length(year, age);
  Double mean_weight = 0.0; //
   mean_weight = length_weight_->mean_weight(size, distribution_, cvs_[age]);
  return mean_weight;
}

} /* namespace agelengths */
} /* namespace niwa */
