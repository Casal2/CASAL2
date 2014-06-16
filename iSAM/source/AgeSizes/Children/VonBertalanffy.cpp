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

// namespaces
namespace isam {
namespace agesizes {

/**
 * default constructor
 */
VonBertalanffy::VonBertalanffy() {
  parameters_.Bind<double>(PARAM_LINF, &linf_, "TBA", "");
  parameters_.Bind<double>(PARAM_K, &k_, "TBA", "");
  parameters_.Bind<double>(PARAM_T0, &t0_, "TBA", "");
  parameters_.Bind<string>(PARAM_SIZE_WEIGHT, &size_weight_label_, "TBA", "");
  parameters_.Bind<double>(PARAM_CV, &cv_, "TBA", "", 0.0);
  parameters_.Bind<string>(PARAM_DISTRIBUTION, &distribution_, "TBA", "", PARAM_NORMAL);
  parameters_.Bind<bool>(PARAM_BY_LENGTH, &by_length_, "TBA", "", true);

  RegisterAsEstimable(PARAM_LINF, &linf_);
  RegisterAsEstimable(PARAM_K, &k_);
  RegisterAsEstimable(PARAM_T0, &t0_);
  RegisterAsEstimable(PARAM_CV, &cv_);

}

/**
 * validate the parameters passed in from the configuration file
 */
void VonBertalanffy::DoValidate() {
  if (linf_ <= 0.0)
    LOG_ERROR(parameters_.location(PARAM_LINF) << "(" << linf_ << ") cannot be less than or equal to 0.0");
  if (k_ <= 0.0)
    LOG_ERROR(parameters_.location(PARAM_K) << "(" << k_ << ") cannot be less than or equal to 0.0");
}

/**
 * build runtime relationships between this object and other objects in the model
 */
void VonBertalanffy::DoBuild() {
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
Double VonBertalanffy::mean_size(unsigned age) const {
  if ((-k_ * (age - t0_)) > 10)
    LOG_ERROR("Fatal error in age-size relationship: exp(-k*(age-t0)) is enormous. The k or t0 parameters are probably wrong.");

  Double size = linf_ * (1 - exp(-k_ * (age - t0_)));
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
Double VonBertalanffy::mean_weight(unsigned age) const {
  Double size = this->mean_size(age);

  Double mean_weight = 0.0;
 if (by_length_) {
   Double cv = (size * cv_) / age;
   mean_weight = size_weight_->mean_weight(size, distribution_, cv);
 } else
   mean_weight = size_weight_->mean_weight(size, distribution_, cv_);

  return mean_weight;
}

} /* namespace agesizes */
} /* namespace isam */
