/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 22/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Factory.h"

#include "Likelihoods/Children/Binomial.h"
#include "Likelihoods/Children/BinomialApprox.h"
#include "Likelihoods/Children/Dirichlet.h"
#include "Likelihoods/Children/LogNormal.h"
#include "Likelihoods/Children/LogNormalWithQ.h"
#include "Likelihoods/Children/Multinomial.h"
#include "Likelihoods/Children/Normal.h"
#include "Likelihoods/Children/Pseudo.h"

// Namespaces
namespace niwa {
namespace likelihoods {

/**
 * Create and return our likelihood
 *
 * @param likelihood_type The type of likelihood to create
 * @return shared_ptr to the likelihood
 */
Likelihood* Factory::Create(const string& likelihood_type) {
  Likelihood* result = nullptr;

  if (likelihood_type == PARAM_BINOMIAL)
    result = new Binomial();
  else if (likelihood_type == PARAM_BINOMIAL_APPROX)
    result = new BinomialApprox();
  else if (likelihood_type == PARAM_DIRICHLET)
    result = new Dirichlet();
  else if (likelihood_type == PARAM_LOGNORMAL)
    result = new LogNormal();
  else if (likelihood_type == PARAM_LOGNORMAL_WITH_Q)
    result = new LogNormalWithQ();
  else if (likelihood_type == PARAM_MULTINOMIAL)
    result = new Multinomial();
  else if (likelihood_type == PARAM_NORMAL)
    result = new Normal();
  else if (likelihood_type == PARAM_PSEUDO)
    result = new Pseudo();

  return result;
}


} /* namespace likelihoods */
} /* namespace niwa */
