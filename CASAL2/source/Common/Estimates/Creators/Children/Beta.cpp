/**
 * @file Beta.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */

// headers
#include "Beta.h"

// namespaces
namespace niwa {
namespace estimates {
namespace creators {

/**
 * Default constructor
 */
Beta::Beta(Model* model) : Creator(model) {
  parameters_.Bind<Double>(PARAM_MU, &mu_, "Beta prior  mean (mu) parameter", "");
  parameters_.Bind<Double>(PARAM_SIGMA, &sigma_, "Beta prior variance (sigma) parameter", "");
  parameters_.Bind<Double>(PARAM_A, &a_, "Beta prior lower bound of the range (A) parameter", "");
  parameters_.Bind<Double>(PARAM_B, &b_, "Beta prior upper bound of the range (B) parameter", "");
}

/**
 *
 */
void Beta::DoCopyParameters(niwa::Estimate* estimate, unsigned index) {
  estimate->parameters().CopyFrom(parameters_, PARAM_MU, index);
  estimate->parameters().CopyFrom(parameters_, PARAM_SIGMA, index);
  estimate->parameters().CopyFrom(parameters_, PARAM_A, index);
  estimate->parameters().CopyFrom(parameters_, PARAM_B, index);
}


} /* namespace creators */
} /* namespace estimates */
} /* namespace niwa */
