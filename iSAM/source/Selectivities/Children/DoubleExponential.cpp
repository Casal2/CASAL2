/**
 * @file DoubleExponential.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 14/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "DoubleExponential.h"

#include "Model/Model.h"

// Namespaces
namespace isam {
namespace selectivities {

/**
 * Default constructor
 */
DoubleExponential::DoubleExponential()
: DoubleExponential(Model::Instance()) {
}

/**
 * Explicit Constructor
 */
DoubleExponential::DoubleExponential(ModelPtr model)
: Selectivity(model) {
  parameters_.Bind<double>(PARAM_X0, &x0_, "X0");
  parameters_.Bind<double>(PARAM_X1, &x1_, "X1");
  parameters_.Bind<double>(PARAM_X2, &x2_, "X2");
  parameters_.Bind<double>(PARAM_Y0, &y0_, "Y0");
  parameters_.Bind<double>(PARAM_Y1, &y1_, "Y1");
  parameters_.Bind<double>(PARAM_Y2, &y2_, "Y2");
  parameters_.Bind<double>(PARAM_ALPHA, &alpha_, "Alpha", 1.0);

  RegisterAsEstimable(PARAM_X0, &x0_);
  RegisterAsEstimable(PARAM_Y0, &y0_);
  RegisterAsEstimable(PARAM_Y1, &y1_);
  RegisterAsEstimable(PARAM_Y2, &y2_);
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
void DoubleExponential::DoValidate() {
  // Param: x0, x1, x2 - Check that x1 is between x0 and x2
  if (x1_ < x0_ || x1_ > x2_)
    LOG_ERROR(parameters_.location(PARAM_X1) << ": x1 ( " << AS_DOUBLE(x1_) << ") must be between x0 (" << AS_DOUBLE(x0_) << ") and x2 (" << AS_DOUBLE(x2_) << ")");

  // Param: y0, y1, y2
  if (y0_ < 0.0)
    LOG_ERROR(parameters_.location(PARAM_Y0) << ": y0 (" << AS_DOUBLE(y0_) << ") is less than 0.0");
  if (y1_ < 0.0)
    LOG_ERROR(parameters_.location(PARAM_Y1) << ": y1 (" << AS_DOUBLE(y1_) << ") is less than 0.0");
  if (y2_ < 0.0)
    LOG_ERROR(parameters_.location(PARAM_Y2) << ": y2 (" << AS_DOUBLE(y2_) << ") is less than 0.0");

  // Param: alpha
  if (alpha_ <= 0.0)
    LOG_ERROR(parameters_.location(PARAM_ALPHA) << ": alpha (" << AS_DOUBLE(alpha_) << ") is less than or equal to 0.0");
}

/**
 * Reset this selectivity so it's ready for the next execution
 * phase in the model.
 *
 * This method will rebuild the cache of selectivity values
 * for each age in the model.
 */
void DoubleExponential::Reset() {
  for (unsigned age = model_->min_age(); age <= model_->max_age(); ++age) {
    if ((double)age <= x0_)
      values_[age] = alpha_ * y0_ * pow((y1_ / y0_), ((double)age - x0_)/(x1_ - x0_));
    else
      values_[age] = alpha_ * y0_ * pow((y2_ / y0_), ((double)age - x0_)/(x1_ - x0_));
  }
}

} /* namespace selectivities */
} /* namespace isam */
