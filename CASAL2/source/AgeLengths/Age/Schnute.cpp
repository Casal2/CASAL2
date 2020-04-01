/**
 * @file Schnute.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 19/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */

// headers
#include "Schnute.h"

#include <cmath>

#include "LengthWeights/Manager.h"
#include "Model/Managers.h"
#include "TimeSteps/Manager.h"
#include "Estimates/Manager.h"
#include "TimeVarying/Manager.h"

// namespaces
namespace niwa {
namespace agelengths {

using std::pow;

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
Schnute::Schnute(Model* model) : AgeLength(model) {
  parameters_.Bind<Double>(PARAM_Y1, &y1_, "The y1 parameter of the Schnute relationship", "");
  parameters_.Bind<Double>(PARAM_Y2, &y2_, "The y2 parameter of the Schnute relationship", "");
  parameters_.Bind<Double>(PARAM_TAU1, &tau1_, "The $\tau_1$ parameter of the Schnute relationship", "");
  parameters_.Bind<Double>(PARAM_TAU2, &tau2_, "The $\tau_2$ parameter of the Schnute relationship", "");
  parameters_.Bind<Double>(PARAM_A, &a_, "The $a$ parameter of the Schnute relationship", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_B, &b_, "The $b$ parameter of the Schnute relationship", "")->set_lower_bound(0.0, false);
  parameters_.Bind<string>(PARAM_LENGTH_WEIGHT, &length_weight_label_, "The label of the associated length-weight relationship", "");

  RegisterAsAddressable(PARAM_Y1, &y1_);
  RegisterAsAddressable(PARAM_Y2, &y2_);
  RegisterAsAddressable(PARAM_TAU1, &tau1_);
  RegisterAsAddressable(PARAM_TAU2, &tau2_);
  RegisterAsAddressable(PARAM_A, &a_);
  RegisterAsAddressable(PARAM_B, &b_);
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 */
void Schnute::DoBuild() {
  length_weight_ = model_->managers().length_weight()->GetLengthWeight(length_weight_label_);
  if (!length_weight_)
    LOG_ERROR_P(PARAM_LENGTH_WEIGHT) << "Length-weight label " << length_weight_label_ << " was not found.";

  // Build up our mean_length_ container.
  unsigned min_age = model_->min_age();
  unsigned max_age = model_->max_age();
  vector<string> time_steps = model_->time_steps();
  for (unsigned step_iter = 0; step_iter < time_steps.size(); ++step_iter) {
    for (unsigned age_iter = min_age; age_iter <= max_age; ++age_iter) {
      mean_length_[step_iter][age_iter] = mean_length(step_iter,age_iter);
    }
  }
}

/**
 * Initialise dependent objects after all objects have been built and validated
 *
 * Check if any of the Schnute age-length parameters are time varying;
 * if so, then make a vector of the unique change years for all parameters
 */
void Schnute::DoInitialise() {
  auto mtv = model_->managers().time_varying();

  auto y1_tv_   = mtv->GetTimeVarying(PARAM_Y1);
  auto y2_tv_   = mtv->GetTimeVarying(PARAM_Y2);
  auto tau1_tv_ = mtv->GetTimeVarying(PARAM_TAU1);
  auto tau2_tv_ = mtv->GetTimeVarying(PARAM_TAU2);
  auto a_tv_    = mtv->GetTimeVarying(PARAM_A);
  auto b_tv_    = mtv->GetTimeVarying(PARAM_B);

  if (y1_tv_->get_years().size() > 0) {
    for (auto val : y1_tv_->get_years())
      time_varying_years_.push_back(val);
  }
  if (y2_tv_->get_years().size() > 0) {
    for (auto val : y2_tv_->get_years())
      time_varying_years_.push_back(val);
  }
  if (tau1_tv_->get_years().size() > 0) {
    for (auto val : tau1_tv_->get_years())
      time_varying_years_.push_back(val);
  }
  if (tau2_tv_->get_years().size() > 0) {
    for (auto val : tau2_tv_->get_years())
      time_varying_years_.push_back(val);
  }
  if (a_tv_->get_years().size() > 0) {
    for (auto val : a_tv_->get_years())
      time_varying_years_.push_back(val);
  }
  if (b_tv_->get_years().size() > 0) {
    for (auto val : b_tv_->get_years())
      time_varying_years_.push_back(val);
  }

  if (time_varying_years_.size() > 0) {
    std::sort(time_varying_years_.begin(), time_varying_years_.end());
    auto omit = std::unique(time_varying_years_.begin(), time_varying_years_.end());
    // remove consecutive adjacent duplicates
    time_varying_years_.erase(omit, time_varying_years_.end());
  }
}

/**
 * Return the mean length for a single population
 *
 * @param time_step The time step
 * @param age The age of the population
 * @return The mean length for one member
 */
Double Schnute::mean_length(unsigned time_step,  unsigned age) {
  Double temp = 0.0;
  Double size = 0.0;

  double proportion = time_step_proportions_[time_step];

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
 * Return the mean weight for a single population
 *
 * @param time_step The time step
 * @param age The age of the population
 * @return mean weight for one member
 */
Double Schnute::mean_weight(unsigned time_step, unsigned age) {
  unsigned year = model_->current_year();
  Double size = mean_length_[time_step][age];
  //LOG_FINE() << "year = " << year << " age " << age << " time step " << time_step << " cv = " <<  cvs_[year][age][time_step];
  Double mean_weight = length_weight_->mean_weight(size, distribution_, cvs_[year][time_step][age]);
  return mean_weight;
}

/**
 * Return the mean length for a time_step and age
 *
 * @param year Ignored for this child (was implemented for the Data AgeLength child)
 * @param time_step The time step
 * @param age The age of the population
 * @return mean weight for one member
 */
Double Schnute::GetMeanLength(unsigned year, unsigned time_step, unsigned age) {
  return mean_length_[time_step][age];
}


/**
 * ReBuildCache: initialised by the timevarying class.
 */
void Schnute::DoRebuildCache() {
  // Re Build up our mean_length_ container.
  unsigned min_age = model_->min_age();
  unsigned max_age = model_->max_age();
  vector<string> time_steps = model_->time_steps();
  for (unsigned step_iter = 0; step_iter < time_steps.size(); ++step_iter) {
    for (unsigned age_iter = min_age; age_iter <= max_age; ++age_iter) {
      mean_length_[step_iter][age_iter] = mean_length(step_iter,age_iter);
    }
  }
}

} /* namespace agelengths */
} /* namespace niwa */
