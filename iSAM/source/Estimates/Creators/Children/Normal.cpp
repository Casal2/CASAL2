/**
 * @file Normal.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Normal.h"

// namespaces
namespace isam {
namespace estimates {
namespace creators {

/**
 * Default constructor
 */
Normal::Normal() {
  parameters_.Bind<Double>(PARAM_MU, &mu_, "", "");
  parameters_.Bind<Double>(PARAM_CV, &cv_, "", "");
}

/**
 *
 */
void Normal::DoCopyParameters(isam::EstimatePtr estimate, unsigned index) {
  estimate->parameters().CopyFrom(parameters_, PARAM_MU, index);
  estimate->parameters().CopyFrom(parameters_, PARAM_CV, index);
}

} /* namespace creators */
} /* namespace estimates */
} /* namespace isam */
