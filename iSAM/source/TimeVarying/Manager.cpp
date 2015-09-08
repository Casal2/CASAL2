/**
 * @file Manager.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 27/01/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Manager.h"

#include "Logging/Logging.h"

// namespaces
namespace niwa {
namespace timevarying {

/**
 *
 */
Manager::Manager() {
}

/**
 *
 */
void Manager::Update(unsigned current_year) {
  LOG_TRACE();
  for (TimeVaryingPtr time_varying : objects_)
    time_varying->Update(current_year);
}

} /* namespace processes */
} /* namespace niwa */
