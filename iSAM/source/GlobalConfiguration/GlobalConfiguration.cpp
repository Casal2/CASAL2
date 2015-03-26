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

#include "Utilities/Logging/Logging.h"
#include "Utilities/To.h"

namespace niwa {

/**
 * Default Constructor
 */
GlobalConfiguration::GlobalConfiguration() {

  global_parameters_[PARAM_DEBUG]                       = "f";
  global_parameters_[PARAM_SKIP_CONFIG_FILE]            = "f";
  global_parameters_[PARAM_CONFIG_FILE]                 = "isam.txt";
  global_parameters_[PARAM_RANDOM_NUMBER_SEED]          = "123";
  global_parameters_[PARAM_FORCE_ESTIMABLE_VALUES_FILE] = "f";
  global_parameters_[PARAM_NO_STANDARD_HEADER_REPORT]   = "f";
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
 * This accessor will return the debug flag.
 *
 * @return If we should run in debug mode of not
 */
bool GlobalConfiguration::debug_mode() {
  bool result = false;

  bool success = util::To<bool>(global_parameters_[PARAM_DEBUG], result);
  if (!success)
    LOG_CODE_ERROR("Could not convert the debug_mode value stored in global configuration to a boolean. The value was " << global_parameters_[PARAM_DEBUG]);

  return result;
}

/**
 * This accessor will return the flag to indicate
 * if we should skip loading a configuration file.
 *
 * This is primarily used when running unit tests and
 * we want to load the configuration from memory.
 *
 * @return flag indicating if we skip configuration file load or not
 */
bool GlobalConfiguration::skip_config_file() {
  bool result = false;

  bool success = util::To<bool>(global_parameters_[PARAM_SKIP_CONFIG_FILE], result);
  if (!success)
    LOG_CODE_ERROR("Could not convert skip_config_file value stored in global configuration to a boolean. The value was " << global_parameters_[PARAM_SKIP_CONFIG_FILE]);

  return result;
}

/**
 * This method will clear any previously set parameters within our global configuration.
 * All member objects need to be cleared during this method.
 */
void GlobalConfiguration::Clear() {
  global_parameters_.clear();
  command_line_parameters_.clear();
}

/**
 * This method will take any values that we've stored from
 * the command line and load them into the global configuration.
 *
 * This is done after we parse the configuration files because
 * some of the information in the configuration files will
 * be modified by the command line.
 */
void GlobalConfiguration::OverrideGlobalValues(const map<string, string>& override_values) {

  for (auto i = override_values.begin(); i != override_values.end(); ++i) {
    parameters_.Add(i->first, i->second, "", 0);
  }
}

/**
 * Get the random number seed from our global configuration
 *
 * @return The random number seed for the system
 */
unsigned GlobalConfiguration::random_seed() {
  return util::ToInline<string, unsigned>(global_parameters_[PARAM_RANDOM_NUMBER_SEED]);
}

/**
 * Return if we are forcing the estimable values file to only allow @estimate
 * defined objects or not.
 */
bool GlobalConfiguration::get_force_estimable_values_file() {
  bool result;
  if (!util::To<bool>(global_parameters_[PARAM_FORCE_ESTIMABLE_VALUES_FILE], result))
    LOG_CODE_ERROR("!util::To<string, bool>(global_parameters_[PARAM_FORCE_ESTIMABLE_VALUES_FILE], result): '" << global_parameters_[PARAM_FORCE_ESTIMABLE_VALUES_FILE] << "'");

  return result;
}

/**
 * Return if we want to print the standard header report or not.
 */
bool GlobalConfiguration::disable_standard_report() {
  bool result;
  if (!util::To<bool>(global_parameters_[PARAM_NO_STANDARD_HEADER_REPORT], result))
    LOG_CODE_ERROR("!util::To<string, bool>(global_parameters_[PARAM_NO_STANDARD_HEADER_REPORT], result): '" << global_parameters_[PARAM_NO_STANDARD_HEADER_REPORT] << "'");

  return result;
}

} /* namespace niwa */
