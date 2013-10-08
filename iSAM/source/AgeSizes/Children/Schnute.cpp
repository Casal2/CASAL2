/*
 * Schnute.cpp
 *
 *  Created on: 24/07/2013
 *      Author: Admin
 */

#include "Schnute.h"

#include <cmath>

#include "SizeWeights/Manager.h"

namespace isam {
namespace agesizes {

using std::pow;

/**
 *
 */
Schnute::Schnute() {
  parameters_.RegisterAllowed(PARAM_Y1, ParameterType::Double, "TBA");
  parameters_.RegisterAllowed(PARAM_Y2, ParameterType::Double, "TBA");
  parameters_.RegisterAllowed(PARAM_TAU1, ParameterType::Double, "TBA");
  parameters_.RegisterAllowed(PARAM_TAU2, ParameterType::Double, "TBA");
  parameters_.RegisterAllowed(PARAM_A, ParameterType::Double, "TBA");
  parameters_.RegisterAllowed(PARAM_B, ParameterType::Double, "TBA");
  parameters_.RegisterAllowed(PARAM_SIZE_WEIGHT, ParameterType::String, "TBA");

  RegisterAsEstimable(PARAM_Y1, &y1_);
  RegisterAsEstimable(PARAM_Y2, &y2_);
  RegisterAsEstimable(PARAM_TAU1, &tau1_);
  RegisterAsEstimable(PARAM_TAU2, &tau2_);
  RegisterAsEstimable(PARAM_A, &a_);
  RegisterAsEstimable(PARAM_B, &b_);
}

/**
 * validate the parameters passed in from the configuration file
 */
void Schnute::DoValidate() {
  CheckForRequiredParameter(PARAM_Y1);
  CheckForRequiredParameter(PARAM_Y2);
  CheckForRequiredParameter(PARAM_TAU1);
  CheckForRequiredParameter(PARAM_TAU2);
  CheckForRequiredParameter(PARAM_A);
  CheckForRequiredParameter(PARAM_B);
  CheckForRequiredParameter(PARAM_SIZE_WEIGHT);

  y1_                 = parameters_.Get(PARAM_Y1).GetValue<double>();
  y2_                 = parameters_.Get(PARAM_Y2).GetValue<double>();
  tau1_               = parameters_.Get(PARAM_TAU1).GetValue<double>();
  tau2_               = parameters_.Get(PARAM_TAU2).GetValue<double>();
  a_                  = parameters_.Get(PARAM_A).GetValue<double>();
  b_                  = parameters_.Get(PARAM_B).GetValue<double>();
  size_weight_label_  = parameters_.Get(PARAM_SIZE_WEIGHT).GetValue<string>();

  if (a_ <= 0.0)
    LOG_ERROR(parameters_.location(PARAM_A) << "(" << a_ << ") cannot be less than or equal to 0.0");
  if (b_ < 1.0)
    LOG_ERROR(parameters_.location(PARAM_B) << "(" << b_ << ") cannot be less than 1.0");
}

/**
 * build runtime relationships between this object and other objects in the model
 */
void Schnute::DoBuild() {
  size_weight_ = sizeweights::Manager::Instance().GetSizeWeight(size_weight_label_);
  if (!size_weight_)
    LOG_ERROR(parameters_.location(PARAM_SIZE_WEIGHT) << "(" << size_weight_label_ << ") could not be found. Have you defined it?");
}

/**
 * Return the mean size for a given age. Mean size returned
 * is for a single fish.
 *
 * @param age The age of the fish to return mean size for
 * @return the mean size for a single fish
 */
Double Schnute::mean_size(unsigned age) const {
  Double temp = 0.0;
  Double size = 0.0;

  if (a_ != 0.0)
    temp = (1 - exp( -a_ * (age - tau1_))) / (1 - exp(-a_ * (tau2_ - tau1_)));
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
Double Schnute::mean_weight(unsigned age) const {
  Double size   = this->mean_size(age);
  return size_weight_->mean_weight(size);
}

} /* namespace agesizes */
} /* namespace isam */
