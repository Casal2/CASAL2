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
namespace niwa {
namespace estimates {
namespace creators {

/**
 * Default constructor
 */
Normal::Normal(shared_ptr<Model> model) : Creator(model) {
  parameters_.Bind<Double>(PARAM_MU, &mu_, "The normal prior mean (mu) parameter", "");
  parameters_.Bind<Double>(PARAM_CV, &cv_, "The normal prior variance (cv) parameter", "");
}

/**
 * Copy the mu and cv parameters
 */
void Normal::DoCopyParameters(niwa::Estimate* estimate, unsigned index) {
  estimate->parameters().CopyFrom(parameters_, PARAM_MU, index);
  estimate->parameters().CopyFrom(parameters_, PARAM_CV, index);
}

} /* namespace creators */
} /* namespace estimates */
} /* namespace niwa */
