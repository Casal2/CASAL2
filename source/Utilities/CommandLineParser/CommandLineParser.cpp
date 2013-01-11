/**
 * @file CommandLineParser.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 16/11/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

 // Headers
#include "CommandLineParser.h"

#include <iostream>
#include <sstream>
#include <boost/program_options.hpp>

#include "GlobalConfiguration/GlobalConfiguration.h"
#include "Model/Model.h"
#include "Utilities/Logging/Logging.h"

// Namespaces
namespace isam {
namespace utilities {

using boost::program_options::options_description;
using boost::program_options::value;
using boost::program_options::variables_map;
using std::cout;
using std::endl;
using std::ostringstream;

/**
 * This method will take the raw command line input from the main() method
 * and process them into something more useful.
 *
 * @param argc The number of arguments that have been provided
 * @param argv Pointer to an array containing the arguments
 */
void CommandLineParser::Parse(int argc, const char* argv[]) {
  LOG_TRACE();

  // Build Options menu
  options_description oDesc("Usage");
  oDesc.add_options()
    ("help,h", "Print help")
    ("license,l", "Display iSAM license")
    ("version,v", "Display version information")
    ("config,c", value<string>(), "Configuration file")
    ("run,r", "Basic model run")
    ("estimate,e", "Point estimation")
    ("input,i", value<string>(), "Load free parameter values from file")
    ("seed,g", value<int>(), "Random number seed")
    ("debug,d", "Run in debug mode (with debug output");

  ostringstream o;
  o << oDesc;
  command_line_usage_ = o.str();

  // Read our Parameters
  variables_map parameters;

  try {
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, oDesc), parameters);
    notify(parameters);

  } catch (boost::program_options::unknown_option &ex) {
    cout << "An error occurred while processing the command line. " << ex.what() << endl;

  }

  /**
   * Load any variables into the global config that need to be available
   * immediately
   */
  GlobalConfigurationPtr global_config = GlobalConfiguration::Instance();
  if (parameters.count("debug"))
    global_config->set_debug_mode("true");
  if (parameters.count("config"))
    global_config->set_config_file(parameters["config"].as<string>());


  /**
   * Determine what run mode we should be in. If we're
   * in help, version or license then we don't need to continue.
   */
  ModelPtr model = Model::Instance();

  if ( (parameters.count("help")) || (parameters.size() == 0) ) {
    model->set_run_mode(RunMode::kHelp);
    return;

  } else if (parameters.count("version")) {
    model->set_run_mode(RunMode::kVersion);
    return;

  } else if (parameters.count("license")) {
    model->set_run_mode(RunMode::kLicense);
    return;
  }

  /**
   * At this point we know we've been asked to do an actual model
   * run. So we need to check to ensure the command line makes
   * sense.
   */
  unsigned run_mode_count = 0;
  run_mode_count += parameters.count("run");
  run_mode_count += parameters.count("estimate");

  if (run_mode_count == 0)
    LOG_ERROR("No valid run mode has been specified on the command line. Please specify a valid run mode (e.g -r)");
  if (run_mode_count > 1)
    LOG_ERROR("Multiple run modes have been specified on the command line. Only 1 run mode is valid");

  if (parameters.count("run"))
    model->set_run_mode(RunMode::kBasic);
  else if (parameters.count("estimate"))
    model->set_run_mode(RunMode::kEstimation);
  else
    LOG_ERROR("An invalid or unknown run mode has been specified on the command line.");

  /**
   * Now we store any variables we want to use to override global defaults.
   */
  if (parameters.count("genseed"))
    override_values_["genseed"] = parameters["genseed"].as<string>();
}

/**
 * This method will take any values that we've stored from
 * the command line and load them into the global configuration.
 *
 * This is done after we parse the configuration files because
 * some of the information in the configuration files will
 * be modified by the command line.
 */
void CommandLineParser::OverrideGlobalValues() {
  LOG_TRACE();

  GlobalConfigurationPtr global_config = GlobalConfiguration::Instance();
  for (auto i = override_values_.begin(); i != override_values_.end(); ++i) {
    global_config->parameters().Add(i->first, i->second, "", 0);
  }
}

} /* namespace utilities */
} /* namespace isam */








