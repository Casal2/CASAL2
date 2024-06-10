/**
 * @file Likelihood.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 10/06/2015
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
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
  // The labels for these complex likelihood cannot be the same as likelihoods that don't require a label
  likelihood_types_with_no_labels_.push_back(PARAM_PSEUDO);
  likelihood_types_with_no_labels_.push_back(PARAM_NORMAL);
  likelihood_types_with_no_labels_.push_back(PARAM_POISSON);
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
