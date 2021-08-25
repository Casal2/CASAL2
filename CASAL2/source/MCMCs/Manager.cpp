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
  if (has_validated_)
    return;

  if (model->run_mode() == RunMode::kMCMC && objects_.size() == 0) {
    LOG_FATAL() << "No @mcmc blocks have been defined, even though this is an MCMC run";
  }

  for (auto mcmc : objects_) mcmc->Validate();

  if (objects_.size() == 1)
    mcmc_ = objects_[0];
  else if (objects_.size() > 1) {
    LOG_FATAL() << "Only one @mcmc block can be specified in a model configuration, you have specified " << objects_.size();
  }

  has_validated_ = true;
}

} /* namespace mcmcs */
} /* namespace niwa */
