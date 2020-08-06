/**
 * @file Likelihood.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 10/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "Likelihood.h"

#include "Model/Model.h"

// namespaces
namespace niwa {

/**
 * Default constructor
 */
Likelihood::Likelihood(Model* model) : model_(model) {
}

/**
 * Validate the parameters
 */
void Likelihood::Validate() {
  LOG_TRACE();
  parameters_.Populate(model_);
  DoValidate();
}

} /* namespace niwa */

