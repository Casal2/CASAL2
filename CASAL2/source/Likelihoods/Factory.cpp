/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 22/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Factory.h"

#include "../Likelihoods/Common/Bernoulli.h"
#include "../Likelihoods/Common/Binomial.h"
#include "../Likelihoods/Common/BinomialApprox.h"
#include "../Likelihoods/Common/Dirichlet.h"
#include "../Likelihoods/Common/LogNormal.h"
#include "../Likelihoods/Common/LogNormalWithQ.h"
#include "../Likelihoods/Common/Multinomial.h"
#include "../Likelihoods/Common/Normal.h"
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
  else if (sub_type == PARAM_LOGNORMAL)
    result = new LogNormal(model);
  else if (sub_type == PARAM_LOGNORMAL_WITH_Q)
    result = new LogNormalWithQ(model);
  else if (sub_type == PARAM_MULTINOMIAL)
    result = new Multinomial(model);
  else if (sub_type == PARAM_NORMAL)
    result = new Normal(model);
  else if (sub_type == PARAM_PSEUDO)
    result = new Pseudo(model);

  if (result)
    model->managers()->likelihood()->AddObject(result);

  return result;
}

} /* namespace likelihoods */
} /* namespace niwa */
