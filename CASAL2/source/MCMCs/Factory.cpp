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

#include "../Model/Model.h"
#include "../Model/Managers.h"
#include "../MCMCs/Manager.h"
#include "../MCMCs/Common/IndependenceMetropolis.h"
//#include "../MCMCs/Common/Kthohr.h"
#include "../MCMCs/Common/RasmussenDunn.h"

// namespaces
namespace niwa {
namespace mcmcs {

/**
 * Factory method
 */
MCMC* Factory::Create(shared_ptr<Model> model, const string& object_type, const string& sub_type) {
  MCMC* object = nullptr;

  if (object_type == PARAM_MCMC) {
    if (sub_type == "" || sub_type == PARAM_INDEPENDENCE_METROPOLIS || sub_type == PARAM_METROPOLIS_HASTINGS)
      object = new IndependenceMetropolis(model);
#ifndef TESTMODE
//    else if (sub_type == PARAM_KTHOHR)
//    	object = new Kthohr(model);
#endif
    else if (sub_type == PARAM_RASMUSSEN_DUNN)
    	object = new RasmussenDunn(model);

  }

  if (object)
    model->managers()->mcmc()->AddObject(object);

  return object;
}

} /* namespace mcmc */
} /* namespace niwa */
