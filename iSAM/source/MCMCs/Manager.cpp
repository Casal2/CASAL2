/**
 * @file Manager.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/05/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "Manager.h"

// namespaces
namespace niwa {
namespace mcmcs {

/**
 *
 */
void Manager::Validate() {
  for (auto mcmc : objects_)
    mcmc->Validate();
}

/**
 *
 */
void Manager::Build() {
  for (auto mcmc : objects_)
    mcmc->Build();

  if (objects_.size() == 1)
    mcmc_ = objects_[0];
  else if (objects_.size() > 1) {
    vector<MCMC*> active_mcmcs;
    for (auto mcmc : objects_) {
      if (mcmc->active())
        active_mcmcs.push_back(mcmc);
    }

    if (active_mcmcs.size() > 1)
      LOG_FATAL() << "Number of active MCMCs is " << active_mcmcs.size() << ". Only 1 MCMC can be defined as active";
    mcmc_ = active_mcmcs[0];
  }
}

} /* namespace mcmcs */
} /* namespace niwa */
