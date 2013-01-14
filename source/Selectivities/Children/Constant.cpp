/*
 * Constant.cpp
 *
 *  Created on: 9/01/2013
 *      Author: Admin
 */

#include "Constant.h"

namespace isam {
namespace selectivities {

/**
 * Default Constructor
 */
Constant::Constant()
: c_(0.0) {

  parameters_.RegisterAllowed(PARAM_C);
}

/**
 * Validate this selectivity
 */
void Constant::Validate() {
  LOG_TRACE();

  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_C);

  label_          = parameters_.Get(PARAM_LABEL).GetValue<string>();
  c_              = parameters_.Get(PARAM_C).GetValue<double>();
}

/**
 * Return the constant result regardless of the
 * age or length specified
 *
 * @param age_or_length unsused in this selectivity
 * @return the constant value
 */
Double Constant::GetResult(unsigned age_or_length) {
  return c_;
}

} /* namespace selectivities */
} /* namespace isam */
