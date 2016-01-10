/**
 * @file Manager.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Dec 7, 2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "Manager.h"

// namespaces
namespace niwa {
namespace estimatetransformations {

/**
 * Validate our estimate transformations
 */
void Manager::Validate() {
  map<string, string> estimate_counts;
  for (auto obj : objects_) {
    obj->Validate();
    set<string> estimates = obj->GetTargetEstimates();
    for (auto estimate : estimates) {
      if (estimate_counts[estimate] != "") {
        LOG_ERROR() << "Estimate " << estimate << " is referenced by both "
            << estimate_counts[estimate] << " and " << obj->label()
            << " transformations";
      }
    }
  }
}

/**
 *
 */
void Manager::TransformEstimates() {
  LOG_TRACE();
  for (auto obj : objects_)
    obj->Transform();
}

/**
 *
 */
void Manager::RestoreEstimates() {
  LOG_TRACE();
  for (auto obj : objects_)
    obj->Restore();
}

} /* namespace estimatetransformations */
} /* namespace niwa */
