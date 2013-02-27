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

  for (EstimateInfoPtr info : estimate_infos_) {
    info->Validate();
    info->BuildEstimates();
  }

  for(EstimatePtr estimate : objects_) {
    estimate->Validate();
  }
}

} /* namespace estimates */
} /* namespace isam */
