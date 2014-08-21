/**
 * @file Manager.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 19/08/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Manager.h"

#include "Utilities/Logging/Logging.h"

// namespaces
namespace isam {
namespace asserts {

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
 * Validate any loaded processes we have.
 */
void Manager::Validate() {
  LOG_TRACE();

}

} /* namespace asserts */
} /* namespace isam */
