/**
 * @file LengthWeight.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/07/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/�2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */

// headers
#include "LengthWeight.h"

#include "../Model/Model.h"

// namespaces
namespace niwa {

/**
 * Default constructor
 */
LengthWeight::LengthWeight(shared_ptr<Model> model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "The label of the length-weight relationship", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "The type of the length-weight relationship", "");
}

/**
 * Validate the LengthWeight objects
 */
void LengthWeight::Validate() {
  parameters_.Populate(model_);
  DoValidate();
}

} /* namespace niwa */
