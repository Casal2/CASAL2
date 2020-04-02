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
#include "LengthWeights/LengthWeight.h"
#include "LengthWeights/Manager.h"
#include "TimeSteps/Manager.h"
#include "Estimates/Manager.h"
#include "TimeVarying/Manager.h"

// namespaces
namespace niwa {
namespace agelengths {

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
    LOG_ERROR_P(PARAM_LENGTH_WEIGHT) << "Length-weight label '" << length_weight_label_ << "' was not found.";

  // Build up our mean_length_ container.
  DoRebuildCache();
}

/**
 * Initialise dependent objects after all objects have been built and validated
 *
 * Check if any of the von Bertalanffy age-length parameters are time varying;
 * if so, then make a vector of the unique change years for all parameters
 */
void VonBertalanffy::DoInitialise() {
  time_varying_years_.clear();

  auto mtv = model_->managers().time_varying();

  auto linf_tv_ = mtv->GetTimeVarying(PARAM_LINF);
  auto k_tv_    = mtv->GetTimeVarying(PARAM_K);
  auto t0_tv_   = mtv->GetTimeVarying(PARAM_T0);

  if (linf_tv_->get_years().size() > 0) {
    for (auto val : linf_tv_->get_years())
      time_varying_years_.push_back(val);
  }
  if (k_tv_->get_years().size() > 0) {
    for (auto val : k_tv_->get_years())
      time_varying_years_.push_back(val);
  }
  if (t0_tv_->get_years().size() > 0) {
    for (auto val : t0_tv_->get_years())
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
 * Return the mean length of a single population
 *
 * @param time_step The time step
 * @param age The age of the population
 * @return The mean length for one member
 */
Double VonBertalanffy::mean_length(unsigned time_step, unsigned age) {
  double proportion = time_step_proportions_[time_step];
  if ((-k_ * ((age + proportion) - t0_)) > 10)
    LOG_ERROR_P(PARAM_K) << "-k*(age-t0) is larger than 10. Check the k and t0 parameters.";

  Double size = linf_ * (1 - exp(-k_ * ((age + proportion) - t0_)));
  if (size < 0.0)
    return 0.0;

  return size;
}

/**
 * Return the mean weight of a single population
 *
 * @param time_step The time step
 * @param age The age of the population
 * @return mean weight for one member
 */
Double VonBertalanffy::mean_weight(unsigned time_step, unsigned age) {
  unsigned year = model_->current_year();
  Double size = mean_length_[time_step][age];
  Double mean_weight = 0.0; //
  mean_weight = length_weight_->mean_weight(size, distribution_, cvs_[year][time_step][age]);// make a map [key = age]
  return mean_weight;
}

/**
 * If time-varying rebuild the cache
 */
void VonBertalanffy::DoReset() {

}

/**
 * Return the mean length for an time_step and age
 *
 * @param year Ignored for this child (was implemented for the Data AgeLength child)
 * @param time_step The time step
 * @param age The age of the population
 * @return mean weight for one member
 */
Double VonBertalanffy::GetMeanLength(unsigned year, unsigned time_step, unsigned age) {
  return mean_length_[time_step][age];
}

/**
 * ReBuildCache: initialised by the timevarying class and build method
 */
void VonBertalanffy::DoRebuildCache() {
  // Re Build up our mean_length_ container.
  unsigned min_age = model_->min_age();
  unsigned max_age = model_->max_age();
  vector<string> time_steps = model_->time_steps();
  for (unsigned step_iter = 0; step_iter < time_steps.size(); ++step_iter) {
    for (unsigned age_iter = min_age; age_iter <= max_age; ++age_iter) {
      mean_length_[step_iter][age_iter] = mean_length(step_iter, age_iter);
    }
  }
}

} /* namespace agelengths */
} /* namespace niwa */
