/**
 * @file AgeLengthKey.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 12/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "AgeLengthKey.h"


// namespaces
namespace niwa {

/**
 * Default constructor
 */
AgeLengthKey::AgeLengthKey() {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type", "");
}

/**
 *
 */
void AgeLengthKey::Validate() {
  LOG_TRACE();

  parameters_.Populate();

  DoValidate();
}

/**
 *
 */
void AgeLengthKey::Build() {
  LOG_TRACE();

  DoBuild();
}


} /* namespace niwa */
