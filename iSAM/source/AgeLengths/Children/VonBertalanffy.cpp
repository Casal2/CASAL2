/**
 * @file VonBertalanffy.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 14/08/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "VonBertalanffy.h"

#include "LengthWeights/Manager.h"
#include "TimeSteps/Manager.h"

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
VonBertalanffy::VonBertalanffy() {
  parameters_.Bind<Double>(PARAM_LINF, &linf_, "TBA", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_K, &k_, "TBA", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_T0, &t0_, "TBA", "");
  parameters_.Bind<string>(PARAM_LENGTH_WEIGHT, &length_weight_label_, "TBA", "");
  parameters_.Bind<Double>(PARAM_CV, &cv_, "TBA", "", 0.0);
  parameters_.Bind<string>(PARAM_DISTRIBUTION, &distribution_, "TBA", "", PARAM_NORMAL);
  parameters_.Bind<bool>(PARAM_BY_LENGTH, &by_length_, "TBA", "", true);

  RegisterAsEstimable(PARAM_LINF, &linf_);
  RegisterAsEstimable(PARAM_K, &k_);
  RegisterAsEstimable(PARAM_T0, &t0_);
  RegisterAsEstimable(PARAM_CV, &cv_);

}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 */
void VonBertalanffy::DoBuild() {
  length_weight_ = lengthweights::Manager::Instance().GetLengthWeight(length_weight_label_);
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
  Double proportion = time_step_proportions_[timesteps::Manager::Instance().current_time_step()];

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

  Double mean_weight = 0.0;
 if (by_length_) {
   mean_weight = length_weight_->mean_weight(size, distribution_, cv_);
 } else {
   Double cv = (age * cv_) / size;
   mean_weight = length_weight_->mean_weight(size, distribution_, cv);
 }

  return mean_weight;
}

} /* namespace agelengths */
} /* namespace niwa */
