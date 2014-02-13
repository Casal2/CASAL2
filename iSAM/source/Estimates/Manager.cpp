/**
 * @file Manager.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 27/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Manager.h"

// Namespaces
namespace isam {
namespace estimates {

/**
 * Default Constructor
 */
Manager::Manager() {
}

/**
 * Destructor
 */
Manager::~Manager() noexcept(true) {
}

/**
 * Validate our Estimate Infos. Then get them to
 * build the Estimates which we will then
 * validate
 */
void Manager::Validate() {
  LOG_TRACE();

  LOG_INFO("Validating EstimateInfo objects then creating estimates");
  for (EstimateInfoPtr info : estimate_infos_) {
    info->Validate();
    info->BuildEstimates();
  }

  LOG_INFO("Validating Estimates");
  for(EstimatePtr estimate : objects_) {
    estimate->Validate();
  }
}

/**
 * Count how many of our estimates are enabled
 * and return the count
 *
 * @return The number of enabled estimates
 */
unsigned Manager::GetEnabledCount() {
  unsigned count = 0;

  for (EstimatePtr estimate : objects_) {
    if (estimate->enabled())
      count++;
  }

  return count;
}

/**
 *
 */
vector<EstimatePtr> Manager::GetEnabled() {
  vector<EstimatePtr> result;

  for (EstimatePtr estimate : objects_) {
    if (estimate->enabled())
      result.push_back(estimate);
  }

  return result;

}

/**
 *
 */
void Manager::RemoveAllObjects() {
  objects_.clear();
  estimate_infos_.clear();
}

} /* namespace estimates */
} /* namespace isam */
