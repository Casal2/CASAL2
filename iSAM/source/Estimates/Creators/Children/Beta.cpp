/**
 * @file Beta.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Beta.h"

// namespaces
namespace isam {
namespace estimates {
namespace creators {

/**
 * Default constructor
 */
Beta::Beta() {
  parameters_.Bind<double>(PARAM_MU, &mu_, "Mu", "");
  parameters_.Bind<double>(PARAM_SIGMA, &sigma_, "Sigma", "");
  parameters_.Bind<double>(PARAM_A, &a_, "A", "");
  parameters_.Bind<double>(PARAM_B, &b_, "B", "");
}

/**
 *
 */
void Beta::DoCopyParameters(isam::EstimatePtr estimate, unsigned index) {
  estimate->parameters().CopyFrom(parameters_, PARAM_MU, index);
  estimate->parameters().CopyFrom(parameters_, PARAM_SIGMA, index);
  estimate->parameters().CopyFrom(parameters_, PARAM_A, index);
  estimate->parameters().CopyFrom(parameters_, PARAM_B, index);
}


} /* namespace creators */
} /* namespace estimates */
} /* namespace isam */
