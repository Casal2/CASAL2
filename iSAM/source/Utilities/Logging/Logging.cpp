/**
 * @file Logging.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 12/11/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Logging.h"
#include "GlobalConfiguration/GlobalConfiguration.h"

#include <iostream>

// Namespaces
namespace niwa {
namespace utilities {

using std::cout;
using std::endl;

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
 * Print the passed in string value to our console.
 *
 * @param value The string we want to print to the console
 */
void Logging::Log(const string &value) {
  if (GlobalConfiguration::Instance()->debug_mode())
    cout << value << endl;
}

void Logging::ForceLog(const string &value) {
  cout << value << endl;
}

} /* namespace utilities */
} /* namespace niwa */
