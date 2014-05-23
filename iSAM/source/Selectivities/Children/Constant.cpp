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
: Selectivity(Model::Instance()) {

  parameters_.Bind<double>(PARAM_C, &c_, "C", "");

  RegisterAsEstimable(PARAM_C, &c_);
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
