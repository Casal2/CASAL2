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
namespace niwa {
namespace estimates {
namespace creators {

/**
 *
 */
Lognormal::Lognormal(Model* model) : Creator(model) {
  parameters_.Bind<Double>(PARAM_MU, &mu_, "The lognormal prior mean (mu) parameter", "");
  parameters_.Bind<Double>(PARAM_CV, &cv_, "The Lognormal variance (CV) parameter", "");
}

/**
 *
 */
void Lognormal::DoCopyParameters(niwa::Estimate* estimate, unsigned index) {
  estimate->parameters().CopyFrom(parameters_, PARAM_MU, index);
  estimate->parameters().CopyFrom(parameters_, PARAM_CV, index);
}

} /* namespace creators */
} /* namespace estimates */
} /* namespace niwa */
