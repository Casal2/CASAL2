/**
 * @file Manager.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/05/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
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
  LOG_TRACE();
  LOG_CODE_ERROR() << "This method is not supported";
}

/**
 *
 */
void Manager::Validate(Model* model) {
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
    unsigned active_count = active_mcmcs.size();

    RunMode::Type must_have_mcmc[] = { RunMode::kMCMC };
    if (std::find(std::begin(must_have_mcmc), std::end(must_have_mcmc), model->run_mode()) != std::end(must_have_mcmc) && active_count != 1) {
      LOG_FATAL() << "You must have one active @mcmc, you have specified " << active_mcmcs.size() << " please fix this.";
    }

    RunMode::Type one_active_only[] = { RunMode::kTesting };
    if (std::find(std::begin(one_active_only), std::end(one_active_only), model->run_mode()) != std::end(one_active_only) && active_count > 1) {
      LOG_FATAL() << "You can only have one active @mcmc, you have specified " << active_mcmcs.size() << " please fix this.";
    }
  }
}

} /* namespace mcmcs */
} /* namespace niwa */
