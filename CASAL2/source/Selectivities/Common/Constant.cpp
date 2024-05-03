/*
 * Constant.cpp
 *
 *  Created on: 9/01/2013
 *      Author: Admin
 */
#include "Constant.h"

#include <boost/math/distributions/lognormal.hpp>
#include <cmath>

#include "../../AgeLengths/AgeLength.h"
#include "../../Model/Model.h"

namespace niwa {
namespace selectivities {

/**
 * Default constructor
 */
Constant::Constant(shared_ptr<Model> model) : Selectivity(model) {
  parameters_.Bind<Double>(PARAM_C, &c_, "The constant value", "");
  parameters_.Bind<Double>(PARAM_BETA, &beta_, "The minimum age for which the selectivity applies", "", 0.0)->set_lower_bound(0.0, true);

  RegisterAsAddressable(PARAM_C, &c_);
  allowed_length_based_in_age_based_model_ = false;
}

void Constant::DoValidate() {
  if (beta_ > model_->max_age())
    LOG_ERROR_P(PARAM_BETA) << ": beta (" << AS_DOUBLE(beta_) << ") cannot be greater than the model maximum age";
}

/**
 * Return the constant result regardless of the
 * age or length specified
 *
 * @param age (unused in this selectivity)
 * @param age_or_length AgeLength pointer (unused in this selectivity)
 * @return the constant value
 */
Double Constant::GetAgeResult(unsigned age, AgeLength* age_length) {
  if (age < beta_)
    return (0.0);
  return c_;
}

Double Constant::get_value(Double value) {
  if (value < beta_)
    return (0.0);
  return c_;
}

Double Constant::get_value(unsigned value) {
  if (value < beta_)
    return (0.0);
  return c_;
}
/**
 * GetLengthResult function
 *
 * @param length_bin_index
 * @return the constant value
 */
Double Constant::GetLengthResult(unsigned length_bin_index) {
  if (length_bin_index < beta_)
    return (0.0);
  return c_;
}

} /* namespace selectivities */
} /* namespace niwa */
