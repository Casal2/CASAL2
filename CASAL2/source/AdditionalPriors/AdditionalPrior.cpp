/**
 * @file AdditionalPrior.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/10/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "AdditionalPrior.h"

#include "../Model/Model.h"

// namespaces
namespace niwa {

/**
 * Default constructor
 *
 * Bind any parameters that are allowed to be loaded from the configuration files.
 * Set bounds on registered parameters
 * Register any parameters that can be an estimated or utilised in other run modes (e.g., profiling, yields, projections, etc.)
 * Set some initial values
 *
 * Note: The constructor is parsed to generate LaTeX for the documentation.
 */
AdditionalPrior::AdditionalPrior(shared_ptr<Model> model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "The label for the additional prior", "");
  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "The name of the parameter for the additional prior", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "The additional prior type", "");
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when we cannot use bind<>() overloads
 *
 * Note: all parameters are populated from configuration files
 */
void AdditionalPrior::Validate() {
  parameters_.Populate(model_);
  DoValidate();
}

/*
 * Build the additional prior
 */
void AdditionalPrior::Build() {
  DoBuild();
}

} /* namespace niwa */
