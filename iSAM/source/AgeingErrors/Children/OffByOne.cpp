/**
 * @file OffByOne.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 19/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "OffByOne.h"

// namespaces
namespace niwa {
namespace ageingerrors {

OffByOne::OffByOne() {
  parameters_.Bind<Double>(PARAM_P1, &p1_, "CV for Misclassification matrix", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_P2, &p2_, "TBA", "");

  RegisterAsEstimable(PARAM_P1, &p1_);
  RegisterAsEstimable(PARAM_P2, &p2_);
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when we cannot use bind<>() overloards
 *
 * Note: all parameters are populated from configuration files
 */
void OffByOne::DoValidate() {

}

void OffByOne::DoBuild() {
  DoReset();
}

void OffByOne::DoReset() {

}

} /* namespace ageingerrors */
} /* namespace niwa */
