/**
 * @file DoubleExponential.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 14/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "DoubleExponential.h"

#include "../../Model/Model.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Explicit Constructor
 */
DoubleExponential::DoubleExponential(shared_ptr<Model> model) : Selectivity(model) {
  parameters_.Bind<Double>(PARAM_X0, &x0_, "The X0 parameter", "");
  parameters_.Bind<Double>(PARAM_X1, &x1_, "The X1 parameter", "");
  parameters_.Bind<Double>(PARAM_X2, &x2_, "The X2 parameter", "");
  parameters_.Bind<Double>(PARAM_Y0, &y0_, "The Y0 parameter", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_Y1, &y1_, "The Y1 parameter", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_Y2, &y2_, "The Y2 parameter", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_ALPHA, &alpha_, "The maximum value of the selectivity", "", 1.0)->set_lower_bound(0.0, false);

  RegisterAsAddressable(PARAM_X0, &x0_);
  RegisterAsAddressable(PARAM_Y0, &y0_);
  RegisterAsAddressable(PARAM_Y1, &y1_);
  RegisterAsAddressable(PARAM_Y2, &y2_);
  RegisterAsAddressable(PARAM_ALPHA, &alpha_);

  allowed_length_based_in_age_based_model_ = true;

}

/**
 * Validate this selectivity. This will load the
 * values that were passed in from the configuration
 * file and assign them to the local variables.
 *
 * Then do some basic checks on the local
 * variables to ensure they are within the business
 * rules for the model.
 */
void DoubleExponential::DoValidate() {
  // Param: x0, x1, x2 - Check that x1 is between x0 and x2
  if (x0_ < x1_ || x0_ > x2_)
    LOG_ERROR_P(PARAM_X0) << "x0 ( " << AS_DOUBLE(x0_) << ") must be between x1 (" << AS_DOUBLE(x1_) << ") and x2 (" << AS_DOUBLE(x2_) << ")";

  // Param: y0, y1, y2
  if (y0_ < 0.0)
    LOG_ERROR_P(PARAM_Y0) << ": y0 (" << AS_DOUBLE(y0_) << ") cannot be less than 0.0";
  if (y1_ < 0.0)
    LOG_ERROR_P(PARAM_Y1) << ": y1 (" << AS_DOUBLE(y1_) << ") cannot be less than 0.0";
  if (y2_ < 0.0)
    LOG_ERROR_P(PARAM_Y2) << ": y2 (" << AS_DOUBLE(y2_) << ") cannot be less than 0.0";

  // Param: alpha
  if (alpha_ <= 0.0)
    LOG_ERROR_P(PARAM_ALPHA) << ": alpha (" << AS_DOUBLE(alpha_) << ") cannot be less than or equal to 0.0";
}
/**
 * The core function
 */
Double DoubleExponential::get_value(Double value) {
  if (value <= x0_) {
    return alpha_ * y0_ * pow((y1_ / y0_), (value - x0_) / (x1_ - x0_));
  } else if (value > x0_ && value <= x2_) {
    return alpha_ * y0_ * pow((y2_ / y0_), (value - x0_) / (x2_ - x0_));
  } else {
    return y2_;
  }
  return 1.0;
}
/**
 * The core function
 */
Double DoubleExponential::get_value(unsigned value) {
  if (value <= x0_) {
    return alpha_ * y0_ * pow((y1_ / y0_), (value - x0_) / (x1_ - x0_));
  } else if (value > x0_ && value <= x2_) {
    return alpha_ * y0_ * pow((y2_ / y0_), (value - x0_) / (x2_ - x0_));
  } else {
    return y2_;
  }
  return 1.0;
}
} /* namespace selectivities */
} /* namespace niwa */
