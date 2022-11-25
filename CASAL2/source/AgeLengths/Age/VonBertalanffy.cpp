/**
 * @file VonBertalanffy.cpp

 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 14/08/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 */

// defines
#define _USE_MATH_DEFINES

// headers
#include "VonBertalanffy.h"

#include <cmath>

#include "../../Estimates/Manager.h"
#include "../../LengthWeights/Manager.h"
#include "../../Model/Managers.h"
#include "../../TimeSteps/Manager.h"

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
VonBertalanffy::VonBertalanffy(shared_ptr<Model> model) : AgeLength(model) {
  parameters_.Bind<Double>(PARAM_LINF, &linf_, "The $L_{infinity}$ parameter", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_K, &k_, "The $k$ parameter", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_T0, &t0_, "The $t_0$ parameter", "");

  RegisterAsAddressable(PARAM_LINF, &linf_);
  RegisterAsAddressable(PARAM_K, &k_);
  RegisterAsAddressable(PARAM_T0, &t0_);
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 */
void VonBertalanffy::DoBuild() {
  length_weight_ = model_->managers()->length_weight()->GetLengthWeight(length_weight_label_);
  if (!length_weight_)
    LOG_ERROR_P(PARAM_LENGTH_WEIGHT) << "Length-weight label '" << length_weight_label_ << "' was not found";

  // Cv by length are we estimating mean growth params
  if ((!IsAddressableUsedFor(PARAM_LINF, addressable::kEstimate) & !IsAddressableUsedFor(PARAM_K, addressable::kEstimate) & !IsAddressableUsedFor(PARAM_T0, addressable::kEstimate))
      & (!IsAddressableUsedFor(PARAM_LINF, addressable::kInputRun) & !IsAddressableUsedFor(PARAM_K, addressable::kInputRun)
         & !IsAddressableUsedFor(PARAM_T0, addressable::kInputRun))) {
    change_mean_length_params_ = false;
  }

  LOG_FINE() << "update CV in reset call = " << change_mean_length_params_;
}

/**
 * If time Varied we need to rebuild the cache
 */
void VonBertalanffy::DoReset() {}

/**
 * This is responsible for returning the correct mean length
 * for this class
 */
Double VonBertalanffy::calculate_mean_length(unsigned year, unsigned time_step, unsigned age) {
  Double proportion = time_step_proportions_[time_step];
  if ((-k_ * ((age + proportion) - t0_)) > 10)
    LOG_ERROR_P(PARAM_K) << "-k*(age-t0) is larger than 10. Please check the k and t0 parameters are sensible";

  Double size = linf_ * (1 - exp(-k_ * ((age + proportion) - t0_)));  // TODO could cache this exp call for all age and proportions
  if (size < 0.0)
    return 0.0;
  // LOG_FINEST() << "age " << age << " size = " << size;
  return size;
}
} /* namespace agelengths */
} /* namespace niwa */
