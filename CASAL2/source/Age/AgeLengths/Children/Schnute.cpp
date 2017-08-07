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

#include "Common/LengthWeights/Manager.h"
#include "Common/Model/Managers.h"
#include "Common/TimeSteps/Manager.h"

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
Schnute::Schnute(Model* model) : AgeLength(model) {
  parameters_.Bind<Double>(PARAM_Y1, &y1_, "Define the y1 parameter of the Schnute relationship", "");
  parameters_.Bind<Double>(PARAM_Y2, &y2_, "Define the y2 parameter of the Schnute relationship", "");
  parameters_.Bind<Double>(PARAM_TAU1, &tau1_, "Define the $\tau_1$ parameter of the Schnute relationship", "");
  parameters_.Bind<Double>(PARAM_TAU2, &tau2_, "Define the $\tau_2$ parameter of the Schnute relationship", "");
  parameters_.Bind<Double>(PARAM_A, &a_, "Define the $a$ parameter of the Schnute relationship", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_B, &b_, "Define the $b$ parameter of the Schnute relationship", "")->set_lower_bound(0.0, false);
  parameters_.Bind<string>(PARAM_LENGTH_WEIGHT, &length_weight_label_, "Define the label of the associated length-weight relationship", "");

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
    LOG_ERROR_P(PARAM_LENGTH_WEIGHT) << "(" << length_weight_label_ << ") could not be found. Have you defined it?";

  // Build up our mean_length_ container.
  unsigned min_age = model_->min_age();
  unsigned max_age = model_->max_age();
  vector<string> time_steps = model_->time_steps();
  for (unsigned step_iter = 0; step_iter < time_steps.size(); ++step_iter) {
    for (unsigned age_iter = min_age; age_iter <= max_age; ++age_iter) {
      mean_length_[step_iter][age_iter] = mean_length(step_iter,age_iter);
      LOG_FINE() << "age_length = " << label_ << " age = " << age_iter << " mean lenght = " << mean_length_[step_iter][age_iter];
    }
  }
}

/**
 * Get the mean length of a single population
 *
 * @param year The year we want mean length for
 * @param age The age of the population we want mean length for
 * @return The mean length for 1 member
 */
Double Schnute::mean_length(unsigned time_step,  unsigned age) {
  Double temp = 0.0;
  Double size = 0.0;

  Double proportion = time_step_proportions_[time_step];

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
 * Get the mean weight of a single population
 *
 * @param year The year we want mean weight for
 * @param age The age of the population we want mean weight for
 * @return mean weight for 1 member
 */
Double Schnute::mean_weight(unsigned time_step, unsigned age) {
  unsigned year = model_->current_year();
  Double size = mean_length_[time_step][age];
  LOG_FINE() << "year = " << year << " age " << age << " time step " << time_step << " cv = " <<  cvs_[year][age][time_step];
  Double mean_weight = length_weight_->mean_weight(size, distribution_, cvs_[year][age][time_step]);
  return mean_weight;
}

} /* namespace agelengths */
} /* namespace niwa */
