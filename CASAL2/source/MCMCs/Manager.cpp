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
void Manager::Validate(shared_ptr<Model> model) {
  if (has_validated_ || model->run_mode() != RunMode::kMCMC)
    return;

  for (auto mcmc : objects_) mcmc->Validate();

  if (objects_.size() == 1)
    mcmc_ = objects_[0];
  else {
    vector<MCMC*> active_mcmcs;
    for (auto mcmc : objects_) {
      if (mcmc->active())
        active_mcmcs.push_back(mcmc);
    }
    unsigned active_count = active_mcmcs.size();

    if (model->run_mode() == RunMode::kMCMC && active_count != 1)
      LOG_FATAL() << "One active @mcmc is required, but " << active_mcmcs.size() << " of " << objects_.size() << " are specified.";
    if (active_mcmcs.size() > 1)
      LOG_FATAL() << "Only One active @mcmc is allowed, but " << active_mcmcs.size() << " of " << objects_.size() << " are specified.";
    if (active_mcmcs.size() == 1)
      mcmc_ = active_mcmcs[0];
  }

  has_validated_ = true;
}

} /* namespace mcmcs */
} /* namespace niwa */
