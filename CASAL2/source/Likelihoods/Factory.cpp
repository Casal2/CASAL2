/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 22/03/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 * Note: when adding a new likelihood if it does NOT require a label please add the type to 'likelihood_types_with_no_labels_' in Likelihood.cpp
 * this prevents users from creating more complex likelihoods with labels the same as 'types'.
 */

// Headers
#include "Factory.h"

#include "../Likelihoods/Common/Bernoulli.h"
#include "../Likelihoods/Common/Binomial.h"
#include "../Likelihoods/Common/BinomialApprox.h"
#include "../Likelihoods/Common/Dirichlet.h"
#include "../Likelihoods/Common/DirichletMultinomial.h"
#include "../Likelihoods/Common/LogNormal.h"
#include "../Likelihoods/Common/LogNormalWithQ.h"
#include "../Likelihoods/Common/Multinomial.h"
#include "../Likelihoods/Common/Normal.h"
#include "../Likelihoods/Common/Poisson.h"
#include "../Likelihoods/Common/Pseudo.h"
#include "../Likelihoods/Manager.h"
#include "../Model/Managers.h"
#include "../Model/Model.h"

// Namespaces
namespace niwa {
namespace likelihoods {

/**
 * Create and return the likelihood
 *
 * @param likelihood_type The type of likelihood to create
 * @return shared_ptr to the likelihood
 */
Likelihood* Factory::Create(shared_ptr<Model> model, const string& object_type, const string& sub_type) {
  Likelihood* result = nullptr;

  if (sub_type == PARAM_BINOMIAL)
    result = new Binomial(model);
  else if (sub_type == PARAM_BERNOULLI)
    result = new Bernoulli(model);
  else if (sub_type == PARAM_BINOMIAL_APPROX)
    result = new BinomialApprox(model);
  else if (sub_type == PARAM_DIRICHLET)
    result = new Dirichlet(model);
  else if (sub_type == PARAM_DIRICHLET_MULTINOMIAL)
    result = new DirichletMultinomial(model);
  else if (sub_type == PARAM_LOGNORMAL)
    result = new LogNormal(model);
  else if (sub_type == PARAM_LOGNORMAL_WITH_Q)
    result = new LogNormalWithQ(model);
  else if (sub_type == PARAM_MULTINOMIAL)
    result = new Multinomial(model);
  else if (sub_type == PARAM_NORMAL)
    result = new Normal(model);
  else if (sub_type == PARAM_POISSON)
    result = new Poisson(model);
  else if (sub_type == PARAM_PSEUDO)
    result = new Pseudo(model);

  if (result)
    model->managers()->likelihood()->AddObject(result);

  return result;
}

} /* namespace likelihoods */
} /* namespace niwa */
