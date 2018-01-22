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
NormalByStdev::NormalByStdev(Model* model) : Creator(model) {
  parameters_.Bind<Double>(PARAM_MU, &mu_, "The normal prior mean (mu) parameter", "");
  parameters_.Bind<Double>(PARAM_SIGMA, &sigma_, "The normal variance (standard devation) parameter", "");
  parameters_.Bind<bool>(PARAM_LOGNORMAL_TRANSFORMATION, &assume_lognormal_, "Add a jacobian if the derived outcome of the estimate is assumed to be lognormal, used for rectuitment deviations in the recruitment process, see the user manual for more information", "", false);

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
