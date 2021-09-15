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

#include "../../Estimates/Manager.h"
#include "../../LengthWeights/Manager.h"
#include "../../Model/Managers.h"
#include "../../TimeSteps/Manager.h"

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
Schnute::Schnute(shared_ptr<Model> model) : AgeLength(model) {
  parameters_.Bind<Double>(PARAM_Y1, &y1_, "The y1 parameter", "");
  parameters_.Bind<Double>(PARAM_Y2, &y2_, "The y2 parameter", "");
  parameters_.Bind<Double>(PARAM_TAU1, &tau1_, "The tau1 parameter", "");
  parameters_.Bind<Double>(PARAM_TAU2, &tau2_, "The tau2 parameter", "");
  parameters_.Bind<Double>(PARAM_A, &a_, "The a parameter", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_B, &b_, "The b parameter", "")->set_lower_bound(0.0, false);
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
  length_weight_ = model_->managers()->length_weight()->GetLengthWeight(length_weight_label_);
  if (!length_weight_)
    LOG_ERROR_P(PARAM_LENGTH_WEIGHT) << "Length-weight label " << length_weight_label_ << " was not found";

  DoRebuildCache();
}

/**
 * Return the mean length for a single population
 *
 * @param time_step The time step
 * @param age The age of the population
 * @return The mean length for one member
 */
Double Schnute::mean_length(unsigned time_step, unsigned age) {
  if (time_step >= time_step_proportions_.size())
    LOG_CODE_ERROR() << "time_step >= time_step_proportions_.size() || " << time_step << " >= " << time_step_proportions_.size();

  Double temp = 0.0;
  Double size = 0.0;

  Double proportion = time_step_proportions_[time_step];

  if (a_ != 0.0)
    temp = (1 - exp(-a_ * ((age + proportion) - tau1_))) / (1 - exp(-a_ * (tau2_ - tau1_)));
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
  Double   size = mean_length_[time_step][age];
  // LOG_FINE() << "year = " << year << " age " << age << " time step " << time_step << " cv = " <<  cvs_[year][age][time_step];
  Double mean_weight = length_weight_->mean_weight(size, distribution_, cvs_[year - year_offset_][time_step - time_step_offset_][age - age_offset_]);
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
 * Return the mean weight for a time_step and age
 *
 * @param year Ignored for this child (was implemented for the Data AgeLength child)
 * @param time_step The time step
 * @param age The age of the population
 * @param length The length of the population
 * @return mean weight for one member
 */
Double Schnute::GetMeanWeight(unsigned year, unsigned time_step, unsigned age, Double length) {
  return length_weight_->mean_weight(length, distribution_, cvs_[year - year_offset_][time_step - time_step_offset_][age - age_offset_]);
}

/**
 * Return the units for the length-weight relationship
 */
string Schnute::weight_units() {
  return length_weight_->weight_units();
}

/**
 * Reset any objects
 */
void Schnute::DoReset() {
  DoRebuildCache();
}

/**
 * ReBuildCache: initialised by the timevarying class.
 */
void Schnute::DoRebuildCache() {
  // Re Build up our mean_length_ container.
  unsigned min_age         = model_->min_age();
  unsigned max_age         = model_->max_age();
  unsigned time_step_count = model_->time_steps().size();
  for (unsigned step_iter = 0; step_iter < time_step_count; ++step_iter) {
    for (unsigned age_iter = min_age; age_iter <= max_age; ++age_iter) {
      mean_length_[step_iter][age_iter] = mean_length(step_iter, age_iter);
    }
  }
}

} /* namespace agelengths */
} /* namespace niwa */
