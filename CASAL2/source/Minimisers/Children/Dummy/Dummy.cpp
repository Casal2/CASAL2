/**
 * @file Dummy.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Feb 9, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 */

// headers
#include "Dummy.h"

// namespaces
namespace niwa {
namespace minimisers {

/**
 * Default constructor
 *
 * @param model Pointer to the model object
 */
Dummy::Dummy(Model* model) : Minimiser(model) {
  parameters_.Add(PARAM_LABEL, "Dummy Minimiser", __FILE__, __LINE__);
  parameters_.Add(PARAM_TYPE, "dummy", __FILE__, __LINE__);
  parameters_.set_ignore_all_parameters();
}


} /* namespace minimisers */
} /* namespace niwa */
