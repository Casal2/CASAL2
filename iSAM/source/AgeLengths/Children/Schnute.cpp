/*
 * Schnute.cpp
 *
 *  Created on: 24/07/2013
 *      Author: Admin
 */

#include "Schnute.h"

#include <cmath>

#include "LengthWeights/Manager.h"
#include "TimeSteps/Manager.h"

namespace niwa {
namespace agelengths {

using std::pow;

/**
 *
 */
Schnute::Schnute() {
  parameters_.Bind<Double>(PARAM_Y1, &y1_, "TBA", "");
  parameters_.Bind<Double>(PARAM_Y2, &y2_, "TBA", "");
  parameters_.Bind<Double>(PARAM_TAU1, &tau1_, "TBA", "");
  parameters_.Bind<Double>(PARAM_TAU2, &tau2_, "TBA", "");
  parameters_.Bind<Double>(PARAM_A, &a_, "TBA", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_B, &b_, "TBA", "")->set_lower_bound(0.0, false);
  parameters_.Bind<string>(PARAM_LENGTH_WEIGHT, &length_weight_label_, "TBA", "");
  parameters_.Bind<Double>(PARAM_CV, &cv_, "TBA", "", 0.0);
  parameters_.Bind<string>(PARAM_DISTRIBUTION, &distribution_, "TBA", "", PARAM_NORMAL);
  parameters_.Bind<bool>(PARAM_BY_LENGTH, &by_length_, "TBA", "", true);

  RegisterAsEstimable(PARAM_Y1, &y1_);
  RegisterAsEstimable(PARAM_Y2, &y2_);
  RegisterAsEstimable(PARAM_TAU1, &tau1_);
  RegisterAsEstimable(PARAM_TAU2, &tau2_);
  RegisterAsEstimable(PARAM_A, &a_);
  RegisterAsEstimable(PARAM_B, &b_);
  RegisterAsEstimable(PARAM_CV, &cv_);
}

/**
 * build runtime relationships between this object and other objects in the model
 */
void Schnute::DoBuild() {
  length_weight_ = lengthweights::Manager::Instance().GetLengthWeight(length_weight_label_);
  if (!length_weight_)
    LOG_ERROR_P(PARAM_LENGTH_WEIGHT) << "(" << length_weight_label_ << ") could not be found. Have you defined it?";
}

/**
 * Return the mean size for a given age. Mean size returned
 * is for a single fish.
 *
 * @param age The age of the fish to return mean size for
 * @return the mean size for a single fish
 */
Double Schnute::mean_length(unsigned year, unsigned age) {
  Double temp = 0.0;
  Double size = 0.0;

  Double proportion = time_step_proportions_[timesteps::Manager::Instance().current_time_step()];

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
 * return the mean weight for a single fish at the given age
 *
 * @param age The age of the fish to return the mean weight for
 * @return The mean weight of a single fish
 */
Double Schnute::mean_weight(unsigned year, unsigned age) {
  Double size   = this->mean_length(year, age);

  Double weight = 0.0;
  if (by_length_) {
    weight = length_weight_->mean_weight(size, distribution_, cv_);
  } else {
    Double cv = (age * cv_) / size;
    weight = length_weight_->mean_weight(size, distribution_, cv);
  }

  return weight;
}

} /* namespace agelengths */
} /* namespace niwa */
