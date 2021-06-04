/**
 * @file SharedLibrary.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 12, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science (c)2016 - www.niwa.co.nz
 */
#include "SharedLibrary.h"

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <iostream>
#include <thread>

#include "ConfigurationLoader/Loader.h"
#include "GlobalConfiguration/GlobalConfiguration.h"
#include "Logging/Logging.h"
#include "Minimisers/Manager.h"
#include "Model/Factory.h"
#include "Model/Managers.h"
#include "Model/Model.h"
#include "Reports/Manager.h"
#include "Runner.h"
#include "Translations/Translations.h"
#include "Utilities/CommandLineParser/CommandLineParser.h"
#include "Utilities/RandomNumberGenerator.h"
#include "Utilities/RunParameters.h"
#include "Utilities/StandardHeader.h"
#include "Version.h"

// Namespaces
using namespace niwa;
using std::cout;
using std::endl;

#ifdef TESTMODE
#include <gtest/gtest.h>

int LoadOptions(int argc, char* argv[], niwa::utilities::RunParameters& options) {
  return -1;
};
int PreParseConfigFiles(niwa::utilities::RunParameters& options) {
  return -1;
};
int Run(int argc, char* argv[], niwa::utilities::RunParameters& options) {
  return -1;
}
int RunUnitTests(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  return result;
}
#else

/**
 * This method runs the unit tests
 */
int RunUnitTests(int argc, char* argv[]) {
  cout << "The shared library was built without TESTMODE enabled but it is trying to run unit tests..." << endl;
  return -1;
}

/**
 * This method loads the command-line arguments
 *
 * @return 0 if successful, -1 if not
 */
int LoadOptions(int argc, char* argv[], niwa::utilities::RunParameters& options) {
  try {
    Logging&                           logging = Logging::Instance();
    niwa::utilities::CommandLineParser parser;
    parser.Parse(argc, argv, options);
    if (logging.errors().size() > 0) {
      logging.FlushErrors();
      return -1;
    }

  } catch (const string& exception) {
    cout << "## ERROR - Casal2 experienced a problem and has stopped execution." << endl;
    cout << "Error: " << exception << endl;
    return -1;
  } catch (std::exception& e) {
    cout << "## ERROR - Casal2 experienced a problem and has stopped execution." << endl;
    cout << e.what() << endl;
    return -1;
  } catch (...) {
    cout << "## ERROR - Casal2 experienced a problem and has stopped execution." << endl;
    cout << "The exception was caught with the catch-all. The error type was unknown." << endl;
    cout << "Please contact the Casal2 application developer." << endl;
    return -1;
  }

  return 0;
}

/**
 * This method is deprecated
 */
int PreParseConfigFiles(niwa::utilities::RunParameters& options) {
  try {
    Logging& logging = Logging::Instance();

    Runner runner;
    runner.global_configuration().set_run_parameters(options);

    // load our configuration file
    configuration::Loader config_loader;
    if (!config_loader.LoadFromDiskToMemory(runner.global_configuration())) {
      logging.FlushErrors();
      return false;
    }

    options.minimiser_  = config_loader.minimiser_type();
    options.model_type_ = config_loader.model_type();

  } catch (const string& exception) {
    cout << "## ERROR - Casal2 experienced a problem and has stopped execution." << endl;
    cout << "Error: " << exception << endl;
    return -1;
  } catch (std::exception& e) {
    cout << "## ERROR - Casal2 experienced a problem and has stopped execution." << endl;
    cout << e.what() << endl;
    return -1;
  } catch (...) {
    cout << "## ERROR - Casal2 experienced a problem and has stopped execution." << endl;
    cout << "The exception was caught with the catch-all. The error type was unknown." << endl;
    cout << "Please contact the Casal2 application developer." << endl;
    return -1;
  }

  return 0;
}

/**
 * This is the main run method for the shared libraries. It is a modified version of main();
 */
int Run(int argc, char* argv[], niwa::utilities::RunParameters& options) {
  int return_code = 0;

  try {
    Runner runner;
    runner.global_configuration().set_run_parameters(options);
    runner.set_run_parameters(options);

    // Store the command line parameters so they can be reported on.
    vector<string> cmd_parameters;
    for (int i = 0; i < argc; ++i) cmd_parameters.push_back(argv[i]);
    runner.global_configuration().set_command_line_parameters(cmd_parameters);

    // Print our standard report header
    if (!options.no_std_report_ && options.print_std_report_header_) {
      utilities::StandardHeader std_report;
      std_report.PrintTop(runner.global_configuration());
      options.std_report_time_ = std_report.start_time();
    }

    return_code = runner.GoWithRunMode(options.run_mode_);

    // pull back the options from the runner incase we've changed any
    // or loaded the MPD data
    options = runner.run_parameters();

    // See if we need to print the end of the standard header footer report
    if (!options.no_std_report_ && options.print_std_report_footer_) {
      utilities::StandardHeader std_report;
      std_report.set_start_time(options.std_report_time_);
      std_report.PrintBottom(runner.global_configuration());
    }

  } catch (const string& exception) {
    cout << "## ERROR - CASAL2 experienced a problem and has stopped execution" << endl;
    cout << "Error: " << exception << endl;
    return_code = -1;

  } catch (std::exception& e) {
    cout << "## ERROR - CASAL2 experienced a problem and has stopped execution" << endl;
    cout << e.what() << endl;
    return_code = -1;

  } catch (...) {
    cout << "## ERROR - CASAL2 experienced a problem and has stopped execution" << endl;
    cout << "The exception was caught with the catch-all. The type was unknown" << endl;
    cout << "Please contact the application developer" << endl;
    return_code = -1;
  }

  if (return_code == 0) {
    LOG_FINEST() << "Done";
  } else {
    LOG_FINEST() << "Done with errors";
  }

  return return_code;
}

#endif
