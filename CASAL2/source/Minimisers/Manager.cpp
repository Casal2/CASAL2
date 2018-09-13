/**
 * @file Manager.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 14/05/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 */

// headers
#include "Manager.h"

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
 * Validate the minimisers
 */
void Manager::Validate() {
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
    if (active_count != 1) {
      LOG_FATAL() << "You can only have one active minimiser per run, you have specified " << active_count << " please fix this.";
    }
  }
}

} /* namespace minimisers */
} /* namespace niwa */
