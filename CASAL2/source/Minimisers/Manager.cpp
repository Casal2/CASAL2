/**
 * @file Manager.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 14/05/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "Manager.h"

#include <algorithm>

// namespaces
namespace niwa {
namespace minimisers {

/**
 * Default constructor
 */
Manager::Manager() {
}

/**
 * Destructor
 */
Manager::~Manager() noexcept(true) {

}

/**
 * Validate any loaded minimisers we have.
 */
void Manager::Validate() {
  LOG_TRACE();
  LOG_CODE_ERROR() << "This method is not supported";
}

void Manager::Validate(Model* model) {
  LOG_TRACE();
  for (auto minimiser : objects_)
    minimiser->Validate();

  if (objects_.size() == 1) {
    objects_[0]->set_active(true);
    active_minimiser_ = objects_[0];

  } else {
    unsigned active_count = 0;
    for (auto minimiser : objects_) {
      if (minimiser->active()) {
        active_count++;
        active_minimiser_ = minimiser;
      }
    }

    RunMode::Type must_have_minimiser[] = { RunMode::kProfiling, RunMode::kEstimation, RunMode::kMCMC };
    if (std::find(std::begin(must_have_minimiser), std::end(must_have_minimiser), model->run_mode()) != std::end(must_have_minimiser) && active_count != 1) {
      LOG_FATAL() << "One active minimiser only per run is required. There are " << active_count << " of " << size() << " specified.";
    }

    RunMode::Type one_active_only[] = { RunMode::kTesting };
    if (std::find(std::begin(one_active_only), std::end(one_active_only), model->run_mode()) != std::end(one_active_only) && active_count > 1) {
      LOG_FATAL() << "One active minimiser only per run is required. There are " << active_count << " of " << size() << " specified.";
    }
  }
}

} /* namespace minimisers */
} /* namespace niwa */
