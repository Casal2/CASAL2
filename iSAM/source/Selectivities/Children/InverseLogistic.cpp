/**
 * @file InverseLogistic.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "InverseLogistic.h"

#include <cmath>

#include "Model/Model.h"

// Namespaces
namespace isam {
namespace selectivities {

/**
 * Default constructor
 */
InverseLogistic::InverseLogistic()
: InverseLogistic(Model::Instance()) {

}

/**
 * Explicit constructor
 */
InverseLogistic::InverseLogistic(ModelPtr model)
: Selectivity(model) {
  parameters_.Bind<double>(PARAM_A50, &a50_, "A50", "");
  parameters_.Bind<double>(PARAM_ATO95, &aTo95_, "aTo95", "");
  parameters_.Bind<double>(PARAM_ALPHA, &alpha_, "Alpha", "", 1.0);

  RegisterAsEstimable(PARAM_A50, &a50_);
  RegisterAsEstimable(PARAM_ATO95, &aTo95_);
  RegisterAsEstimable(PARAM_ALPHA, &alpha_);
}

/**
 * Validate this selectivity. This will load the
 * values that were passed in from the configuration
 * file and assign them to the local variables.
 *
 * We'll then do some basic checks on the local
 * variables to ensure they are within the business
 * rules for the model.
 */
void InverseLogistic::DoValidate() {
  if (alpha_ <= 0.0)
    LOG_ERROR(parameters_.location(PARAM_ALPHA) << ": alpha (" << AS_DOUBLE(alpha_) << ") cannot be less than or equal to 0.0");
  if (aTo95_ <= 0.0)
    LOG_ERROR(parameters_.location(PARAM_ATO95) << ": ato95 (" << AS_DOUBLE(aTo95_) << ") cannot be less than or equal to 0.0");
}

/**
 * Reset this selectivity so it's ready for the next execution
 * phase in the model.
 *
 * This method will rebuild the cache of selectivity values
 * for each age in the model.
 */
void InverseLogistic::Reset() {
  Double threshold = 0.0;

  for (unsigned age = model_->min_age(); age <= model_->max_age(); ++age) {
	  Double temp = (double)age;
    threshold = (Double)(a50_ - temp) / aTo95_;

    if (threshold > 5.0)
      values_[age] = alpha_;
    else if (threshold < -5.0)
      values_[age] = 0.0;
    else
      values_[age] = alpha_ - (alpha_ / (1.0 + pow(19.0, threshold)));
  }
}

} /* namespace selectivities */
} /* namespace isam */
