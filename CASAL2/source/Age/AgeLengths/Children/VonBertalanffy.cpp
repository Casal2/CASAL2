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

#include "Common/Model/Managers.h"
#include "Common/LengthWeights/LengthWeight.h"
#include "Common/LengthWeights/Manager.h"
#include "Common/TimeSteps/Manager.h"

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
VonBertalanffy::VonBertalanffy(Model* model) : AgeLength(model) {
  parameters_.Bind<Double>(PARAM_LINF, &linf_, "Define the $L_{infinity}$ parameter of the von Bertalanffy relationship", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_K, &k_, "Define the $k$ parameter of the von Bertalanffy relationship", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_T0, &t0_, "Define the $t_0$ parameter of the von Bertalanffy relationship", "");
  parameters_.Bind<string>(PARAM_LENGTH_WEIGHT, &length_weight_label_, "Define the label of the associated length-weight relationship", "");
//  parameters_.Bind<bool>(PARAM_BY_LENGTH, &by_length_, "Specifies if the linear interpolation of CV's is a linear function of mean length at age. Default is just by age", "", true);

  RegisterAsAddressable(PARAM_LINF, &linf_);
  RegisterAsAddressable(PARAM_K, &k_);
  RegisterAsAddressable(PARAM_T0, &t0_);
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 */
void VonBertalanffy::DoBuild() {
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
Double VonBertalanffy::mean_length(unsigned time_step, unsigned age) {
  Double proportion = time_step_proportions_[time_step];
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
Double VonBertalanffy::mean_weight(unsigned time_step, unsigned age) {
  unsigned year = model_->current_year();
  Double size = mean_length_[time_step][age];
  Double mean_weight = 0.0; //
  mean_weight = length_weight_->mean_weight(size, distribution_, cvs_[year][age][time_step]);// make a map [key = age]
  return mean_weight;
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 */
void VonBertalanffy::DoReset() {
  // ReBuild our mean_length_ container.
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
