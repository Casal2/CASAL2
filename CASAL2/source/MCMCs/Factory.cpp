/**
 * @file Factory.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 20/05/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */

// headers
#include "Factory.h"

#include "../MCMCs/Common/IndependenceMetropolis.h"
#include "../MCMCs/Common/RandomWalkMetropolisHastings.h"
#include "../MCMCs/Manager.h"
#include "../Model/Managers.h"
#include "../Model/Model.h"

// namespaces
namespace niwa {
namespace mcmcs {

/**
 * Factory method
 */
MCMC* Factory::Create(shared_ptr<Model> model, const string& object_type, const string& sub_type) {
  MCMC* object = nullptr;
#ifndef USE_AUTODIFF
  if (object_type == PARAM_MCMC) {
    if (sub_type == "" || sub_type == PARAM_INDEPENDENCE_METROPOLIS || sub_type == PARAM_METROPOLIS_HASTINGS)
      object = new IndependenceMetropolis(model);
    else if (sub_type == PARAM_RANDOMWALK)
      object = new RandomWalkMetropolisHastings(model);
  }

  if (object)
    model->managers()->mcmc()->AddObject(object);
#endif  // USE_AUTODIFF
  return object;
}

}  // namespace mcmcs
} /* namespace niwa */
