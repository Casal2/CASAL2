/**
 * @file Factory.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 20/05/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "Factory.h"

#include "MCMCs/Manager.h"
#include "MCMCs/Children/IndependenceMetropolis.h"

// namespaces
namespace niwa {
namespace mcmcs {

/**
 * Factory method
 */
MCMC* Factory::Create(const string& object_type, const string& sub_type) {
  MCMC* object = nullptr;

  if (object_type == PARAM_MCMC) {
    if (sub_type == "" || sub_type == PARAM_INDEPENDENCE_METROPOLIS || sub_type == PARAM_METROPOLIS_HASTINGS)
      object = new IndependenceMetropolis();
  }

  if (object)
    mcmcs::Manager::Instance().AddObject(object);

  return object;
}

} /* namespace mcmc */
} /* namespace niwa */
