/**
 * @file Project.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/05/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Project.h"

// namespaces
namespace isam {

/**
 * Default constructor
 */
Project::Project() {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type", "", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years to recalculate the values", "", true);
}

/**
 *
 */
void Project::Validate() {
  parameters_.Populate();
  DoValidate();
}

/**
 *
 */
void Project::Build() {
}

} /* namespace isam */
