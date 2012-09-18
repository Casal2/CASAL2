/**
 * @file RuntimeController.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "RuntimeController.h"
#include "../Utilities/Exception.h"

#include <iostream>
#include <sstream>
#include <boost/program_options.hpp>

// Namespaces
using boost::program_options::options_description;
using boost::program_options::value;
using boost::program_options::variables_map;
using std::cout;
using std::endl;
using std::ostringstream;

namespace iSAM {

/**
 * Default Constructor
 */
RuntimeController::RuntimeController() {

  // Assign some variables
  pGlobalConfig = GlobalConfiguration::getInstance();
  eRunMode      = RunMode::INVALID;

}

/**
 * Destructor
 */
RuntimeController::~RuntimeController() {
}

/**
 * Generic single method to return the static instance.
 *
 * @return shared_ptr to the static instance
 */
shared_ptr<RuntimeController> RuntimeController::getInstance() {
  RuntimeControllerPtr instance = RuntimeControllerPtr(new RuntimeController());
  return instance;
}

/**
 * This method will take the raw command line input from the main() method
 * and process them into something more useful.
 *
 * @param argc The number of arguments that have been provided
 * @param argv Pointer to an array containing the arguments
 */
void RuntimeController::parseCommandLine(int argc, const char* argv[]) {

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
  sCommandLineUsage = o.str();

  // Read our Parameters
  variables_map vmParams;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, oDesc), vmParams);
  notify(vmParams);

  /**
   * Determine what run mode we should be in. If we're
   * in help, version or license then we don't need to continue.
   */
  if ( (vmParams.count("help")) || (vmParams.size() == 0) ) {
    eRunMode = RunMode::HELP;
    return;

  } else if (vmParams.count("version")) {
    eRunMode = RunMode::VERSION;
    return;

  } else if (vmParams.count("license")) {
    eRunMode = RunMode::LICENSE;
    return;
  }

  /**
   * At this point we know we've been asked to do an actual model
   * run. So we need to check to ensure the command line makes
   * sense.
   */
  unsigned runModeCount = 0;
  runModeCount += vmParams.count("run");
  runModeCount += vmParams.count("estimate");

  if (runModeCount == 0)
    THROW_EXCEPTION("No valid run modes have been specified");
  if (runModeCount > 1)
    THROW_EXCEPTION("More than 1 run mode has been specified");

  if (vmParams.count("run"))
    eRunMode = RunMode::BASIC;
  else if (vmParams.count("estimate"))
    eRunMode = RunMode::ESTIMATION;
  else
    THROW_EXCEPTION("Invalid run mode has been specified, or this run mode is not currently supported");

  /**
   * TODO: Load the Free-Parameters into the Estimate system
   */

  if (vmParams.count("debug"))
    pGlobalConfig->setDebugMode("true");
  if (vmParams.count("genseed"))
    pGlobalConfig->setRandomSeed(vmParams["genseed"].as<string>());
  if (vmParams.count("file"))
    pGlobalConfig->setConfigFile(vmParams["file"].as<string>());
}

} /* namespace iSAM */







































