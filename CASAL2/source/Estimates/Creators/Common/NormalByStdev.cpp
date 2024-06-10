/**
 * @file NormalByStdev.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/09/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
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
NormalByStdev::NormalByStdev(shared_ptr<Model> model) : Creator(model) {
  parameters_.Bind<Double>(PARAM_MU, &mu_, "The normal prior mean (mu) parameter", "");
  parameters_.Bind<Double>(PARAM_SIGMA, &sigma_, "The normal standard deviation (sigma) parameter", "");
  parameters_.Bind<bool>(
      PARAM_LOGNORMAL_TRANSFORMATION, &assume_lognormal_,
      "Add a jacobian if the derived outcome of the estimate is assumed to be lognormal, e.g., used for recruitment deviations in the recruitment process. See the User Manual for more information",
      "", false);
}

/**
 *
 */
void NormalByStdev::DoCopyParameters(niwa::Estimate* estimate, unsigned index) {
  estimate->parameters().CopyFrom(parameters_, PARAM_MU, index);
  estimate->parameters().CopyFrom(parameters_, PARAM_SIGMA, index);
  estimate->parameters().CopyFrom(parameters_, PARAM_LOGNORMAL_TRANSFORMATION);
}
} /* namespace creators */
} /* namespace estimates */
} /* namespace niwa */
