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

#include "SizeWeights/Manager.h"
#include "TimeSteps/Manager.h"

// namespaces
namespace niwa {
namespace agelengths {

/**
 * default constructor
 */
VonBertalanffy::VonBertalanffy() {
  parameters_.Bind<Double>(PARAM_LINF, &linf_, "TBA", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_K, &k_, "TBA", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_T0, &t0_, "TBA", "");
  parameters_.Bind<string>(PARAM_SIZE_WEIGHT, &size_weight_label_, "TBA", "");
  parameters_.Bind<Double>(PARAM_CV, &cv_, "TBA", "", 0.0);
  parameters_.Bind<string>(PARAM_DISTRIBUTION, &distribution_, "TBA", "", PARAM_NORMAL);
  parameters_.Bind<bool>(PARAM_BY_LENGTH, &by_length_, "TBA", "", true);

  RegisterAsEstimable(PARAM_LINF, &linf_);
  RegisterAsEstimable(PARAM_K, &k_);
  RegisterAsEstimable(PARAM_T0, &t0_);
  RegisterAsEstimable(PARAM_CV, &cv_);

}

/**
 * build runtime relationships between this object and other objects in the model
 */
void VonBertalanffy::DoBuild() {
  size_weight_ = sizeweights::Manager::Instance().GetSizeWeight(size_weight_label_);
  if (!size_weight_)
    LOG_ERROR_P(PARAM_SIZE_WEIGHT) << "(" << size_weight_label_ << ") could not be found. Have you defined it?";
}

/**
 * Return the mean size for a given age. Mean size returned
 * is for a single fish.
 *
 * @param age The age of the fish to return mean size for
 * @return the mean size for a single fish
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
 * return the mean weight for a single fish at the given age
 *
 * @param age The age of the fish to return the mean weight for
 * @return The mean weight of a single fish
 */
Double VonBertalanffy::mean_weight(unsigned year, unsigned age) {
  Double size = this->mean_length(year, age);

  Double mean_weight = 0.0;
 if (by_length_) {
   mean_weight = size_weight_->mean_weight(size, distribution_, cv_);
 } else {
   Double cv = (age * cv_) / size;
   mean_weight = size_weight_->mean_weight(size, distribution_, cv);
 }

  return mean_weight;
}

} /* namespace agelengths */
} /* namespace niwa */
