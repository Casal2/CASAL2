/**
 * @file Lognormal.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Lognormal.h"

// namespaces
namespace isam {
namespace estimates {
namespace creators {

/**
 *
 */
Lognormal::Lognormal() {
  parameters_.Bind<double>(PARAM_MU, &mu_, "", "");
  parameters_.Bind<double>(PARAM_CV, &cv_, "", "");
}

/**
 *
 */
void Lognormal::DoCopyParameters(isam::EstimatePtr estimate, unsigned index) {
  estimate->parameters().CopyFrom(parameters_, PARAM_MU, index);
  estimate->parameters().CopyFrom(parameters_, PARAM_CV, index);
}

} /* namespace creators */
} /* namespace estimates */
} /* namespace isam */
