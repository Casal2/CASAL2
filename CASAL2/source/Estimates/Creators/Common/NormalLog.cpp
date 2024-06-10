/**
 * @file NormalLog.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/09/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/�2014 - www.niwa.co.nz
 *
 */

// headers
#include "NormalLog.h"

// namespaces
namespace niwa {
namespace estimates {
namespace creators {

/**
 * Default constructor
 */
NormalLog::NormalLog(shared_ptr<Model> model) : Creator(model) {
  parameters_.Bind<Double>(PARAM_MU, &mu_, "The normal-log prior mean (mu) parameter", "");
  parameters_.Bind<Double>(PARAM_SIGMA, &sigma_, "The normal-log prior standard deviation (sigma) parameter", "");
}

/**
 * Copy the mu and sigma parameters
 */
void NormalLog::DoCopyParameters(niwa::Estimate* estimate, unsigned index) {
  estimate->parameters().CopyFrom(parameters_, PARAM_MU, index);
  estimate->parameters().CopyFrom(parameters_, PARAM_SIGMA, index);
}

} /* namespace creators */
} /* namespace estimates */
} /* namespace niwa */
