/*
 * Constant.cpp
 *
 *  Created on: 9/01/2013
 *      Author: Admin
 */

#include <boost/math/distributions/lognormal.hpp>
#include <Selectivities/Common/Constant.h>
#include <cmath>

#include "AgeLengths/AgeLength.h"
#include "Model/Model.h"

namespace niwa {
namespace selectivities {

/**
 * Default Constructor
 */
Constant::Constant(Model* model)
: Selectivity(model) {

  parameters_.Bind<Double>(PARAM_C, &c_, "C", "");

  RegisterAsAddressable(PARAM_C, &c_);
}

/**
 * Reset this selectivity so it's ready for the next execution
 * phase in the model.
 *
 * This method will rebuild the cache of selectivity values
 * for each age in the model.
 */
void Constant::RebuildCache() {
  values_.assign(model_->age_spread(), c_);
}

/**
 * Return the constant result regardless of the
 * age or length specified
 *
 * @param age_or_length unsused in this selectivity
 * @return the constant value
 */
Double Constant::GetAgeResult(unsigned age, AgeLength* age_length) {
  return c_;
}

/**
 * GetLengthBasedResult function
 *
 * @param age
 * @param age_length AgeLength pointer
 * @return Double selectivity for an age based on age length distribution
 */

Double Constant::GetLengthBasedResult(unsigned age, AgeLength* age_length, unsigned year, int time_step_index) {
  return c_;
}

/**
 *
 */
Double Constant::GetLengthResult(unsigned length_bin_index) {
  return c_;
}

} /* namespace selectivities */
} /* namespace niwa */
