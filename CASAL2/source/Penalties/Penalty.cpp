/**
 * @file Penalty.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Penalty.h"

#include "Penalties/Manager.h"
#include "Utilities/DoubleCompare.h"

// Namespaces
namespace niwa {

/**
 * Default Constructor
 */
Penalty::Penalty(Model* model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "The label of the penalty", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "The type of penalty", "");

}

/**
 * Validate our penalty parameters
 */
void Penalty::Validate() {
  parameters_.Populate();
  DoValidate();
}

} /* namespace niwa */
