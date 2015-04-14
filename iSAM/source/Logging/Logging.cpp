/**
 * @file Logging.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 14/04/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "Logging.h"

// namespaces
namespace niwa {

/**
 * Singleton Instance method
 *
 * @return static instance of the logging class
 */
Logging& Logging::Instance() {
  static Logging logging;
  return logging;
}

/**
 *
 */
void Logging::Flush(niwa::logger::Record& record) {

}

} /* namespace niwa */
