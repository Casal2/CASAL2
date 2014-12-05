/**
 * @file NormalByStdev.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "NormalByStdev.h"

// namespaces
namespace niwa {
namespace estimates {
namespace creators {

/**
 *
 */
NormalByStdev::NormalByStdev() {
  parameters_.Bind<Double>(PARAM_MU, &mu_, "", "");
  parameters_.Bind<Double>(PARAM_SIGMA, &sigma_, "", "");
}

/**
 *
 */
void NormalByStdev::DoCopyParameters(niwa::EstimatePtr estimate, unsigned index) {
  estimate->parameters().CopyFrom(parameters_, PARAM_MU, index);
  estimate->parameters().CopyFrom(parameters_, PARAM_SIGMA, index);
}
} /* namespace creators */
} /* namespace estimates */
} /* namespace niwa */
