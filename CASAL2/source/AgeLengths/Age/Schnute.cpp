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
  // Cv by length are we estiamteing mean growth params
  // if we aren't estimating or using in -i 
  LOG_FINE() << "does a have an @estimate block " << IsAddressableUsedFor(PARAM_A, addressable::kEstimate);
  if((!IsAddressableUsedFor(PARAM_Y1, addressable::kEstimate) & !IsAddressableUsedFor(PARAM_Y2, addressable::kEstimate) & !IsAddressableUsedFor(PARAM_A, addressable::kEstimate) & !IsAddressableUsedFor(PARAM_B, addressable::kEstimate)) & 
     (!IsAddressableUsedFor(PARAM_Y1, addressable::kInputRun) & !IsAddressableUsedFor(PARAM_Y2, addressable::kInputRun) & !IsAddressableUsedFor(PARAM_A, addressable::kInputRun) & !IsAddressableUsedFor(PARAM_B, addressable::kInputRun))) {
    change_mean_length_params_ = false;
    LOG_FINE() << "Not estimating mean length params and cv by length so don't need to update CV or Length age translation";
  }

  LOG_FINE() << "update CV in reset call = " << change_mean_length_params_;
}


/**
 * Reset any objects
 */
void Schnute::DoReset() {

}


/**
 * This is responsible for returning the correct mean length
 * for this class
 */
Double Schnute::calculate_mean_length(unsigned year, unsigned time_step, unsigned age) {
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
  //LOG_FINEST() << "age " << age << " size = " << size;

  return size;
}
} /* namespace agelengths */
} /* namespace niwa */
