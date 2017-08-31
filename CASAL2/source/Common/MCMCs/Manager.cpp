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

  if (objects_.size() == 1)
    mcmc_ = objects_[0];
  else {
    vector<MCMC*> active_mcmcs;
    for (auto mcmc : objects_) {
      if (mcmc->active())
        active_mcmcs.push_back(mcmc);
    }
    if (active_mcmcs.size() != 1) {
      LOG_FATAL() << "You can only have one active @mcmc per run, you have specified " << active_mcmcs.size() << " please fix this.";
    }
  }
}

} /* namespace mcmcs */
} /* namespace niwa */
