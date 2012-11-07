/**
 * @file GlobalConfiguration.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "GlobalConfiguration.h"

namespace isam {

/**
 * Default Constructor
 */
GlobalConfiguration::GlobalConfiguration() {
}

/**
 * Destructor
 */
GlobalConfiguration::~GlobalConfiguration() {
}

/**
 * Standard singletone instance method
 *
 * @return static shared_ptr<> to the instance
 */
shared_ptr<GlobalConfiguration> GlobalConfiguration::Instance() {
  static GlobalConfigurationPtr instance = GlobalConfigurationPtr(new GlobalConfiguration());
  return instance;
}

/**
 * This method will clear any previously set parameters within our global configuration.
 * All member objects need to be cleared during this method.
 */
void GlobalConfiguration::Clear() {
  parameters_.clear();
  command_line_parameters_.clear();
}

} /* namespace isam */
