/**
 * @file Likelihood.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 10/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */

// headers
#include "Likelihood.h"

#include "../Model/Model.h"

// namespaces
namespace niwa {

/**
 * Default constructor
 */
Likelihood::Likelihood(shared_ptr<Model> model) : model_(model) {
  // more complex likelihoods (have estimable parameters) have label subcommands. 
  // These labels cannot be the 
  likelihood_types_with_no_labels_.push_back(PARAM_PSEUDO);
  likelihood_types_with_no_labels_.push_back(PARAM_NORMAL);
  likelihood_types_with_no_labels_.push_back(PARAM_MULTINOMIAL);
  likelihood_types_with_no_labels_.push_back(PARAM_LOGNORMAL_WITH_Q);
  likelihood_types_with_no_labels_.push_back(PARAM_LOGNORMAL);
  likelihood_types_with_no_labels_.push_back(PARAM_DIRICHLET);
  likelihood_types_with_no_labels_.push_back(PARAM_BINOMIAL_APPROX);
  likelihood_types_with_no_labels_.push_back(PARAM_BINOMIAL);
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
