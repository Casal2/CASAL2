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

#include "Common/Model/Model.h"
#include "Common/Model/Managers.h"
#include "Common/Likelihoods/Manager.h"
#include "Common/Likelihoods/Children/Binomial.h"
#include "Common/Likelihoods/Children/BinomialApprox.h"
#include "Common/Likelihoods/Children/Dirichlet.h"
#include "Common/Likelihoods/Children/LogNormal.h"
#include "Common/Likelihoods/Children/LogNormalWithQ.h"
#include "Common/Likelihoods/Children/Multinomial.h"
#include "Common/Likelihoods/Children/Normal.h"
#include "Common/Likelihoods/Children/Pseudo.h"

// Namespaces
namespace niwa {
namespace likelihoods {

/**
 * Create and return our likelihood
 *
 * @param likelihood_type The type of likelihood to create
 * @return shared_ptr to the likelihood
 */
Likelihood* Factory::Create(Model* model, const string& object_type, const string& sub_type) {
  Likelihood* result = nullptr;

  if (sub_type == PARAM_BINOMIAL)
    result = new Binomial(model);
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
    model->managers().likelihood()->AddObject(result);

  return result;
}


} /* namespace likelihoods */
} /* namespace niwa */
