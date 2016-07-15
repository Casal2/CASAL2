/**
 * @file Basic.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "Basic.h"

// namespaces
namespace niwa {
namespace lengthweights {

/**
 * default constructor
 */
Basic::Basic() {
  parameters_.Bind<Double>(PARAM_A, &a_, "A", "");
  parameters_.Bind<Double>(PARAM_B, &b_, "B", "");
  parameters_.Bind<string>(PARAM_UNITS, &units_, "Units of measure (tonnes, kgs, grams)", "");
}

/**
 * validate the values passed in from the configuration
 * file. Check that neither 'a' or 'b' are greater
 * than 0.0
 */
void Basic::DoValidate() {
  if (a_ <= 0.0)
    LOG_ERROR_P(PARAM_A) << " (" << AS_DOUBLE(a_) << ") cannot be less than or equal to 0.0";
  if (b_ <= 0.0)
    LOG_ERROR_P(PARAM_B) << " (" << AS_DOUBLE(b_) << ") cannot be less than or equal to 0.0";

  if (units_ != PARAM_TONNES && units_ != PARAM_KGS && units_ != PARAM_GRAMS)
    LOG_ERROR_P(PARAM_UNITS) << " (" << units_ << ") is not supported. Supported units are: tonnes, kgs, grams";

}

/**
 * Calculate and return the mean weight for the size of fish passed in.
 *
 * @param size The size of the population to calculate mean weight for
 * @return The mean weight of the population at the parameter size
 */
Double Basic::mean_weight(Double size, const string &distribution, Double cv) const {
  Double weight = a_ * pow(size, b_);
  if (units_ == PARAM_TONNES)
    weight *= 1000;
  if (units_ == PARAM_GRAMS)
    weight /= 1000;
  if (distribution == PARAM_NORMAL || distribution == PARAM_LOGNORMAL)
    weight = weight * pow(1.0 + cv * cv, b_ * (b_ - 1.0) / 2.0);  // Give an R example/proof of this theory

  return weight;
}

} /* namespace lengthweights */
} /* namespace niwa */
