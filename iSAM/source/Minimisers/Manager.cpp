/*
 * Manager.cpp
 *
 *  Created on: 28/02/2013
 *      Author: Admin
 */

#include "Manager.h"

namespace isam {
namespace minimisers {

Manager::Manager() {
  // TODO Auto-generated constructor stub

}

Manager::~Manager() noexcept(true) {
  // TODO Auto-generated destructor stub
}

void Manager::Validate() {
  for (MinimiserPtr minimiser : objects_)
    minimiser->Validate();


  if (objects_.size() == 1) {
    objects_[0]->set_active(true);
    active_minimiser_ = objects_[0];

  } else {
    unsigned active_count = 0;
    for (MinimiserPtr minimiser : objects_) {
      if (minimiser->active()) {
        active_count++;
        active_minimiser_ = minimiser;
      }
    }

    if (active_count > 1) {
      LOG_ERROR("More than one @minimiser block has active set to true. You can only have one active minimiser per run");
    }
  }
}

} /* namespace minimisers */
} /* namespace isam */
