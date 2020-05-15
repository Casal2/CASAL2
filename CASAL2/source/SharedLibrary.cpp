/**
 * @file SharedLibrary.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 12, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science �2016 - www.niwa.co.nz
 */
#include "SharedLibrary.h"

#include <iostream>
#include <thread>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>

#include "Version.h"
#include "ConfigurationLoader/Loader.h"
#include "GlobalConfiguration/GlobalConfiguration.h"
#include "Model/Factory.h"
#include "Model/Managers.h"
#include "Model/Model.h"
#include "Minimisers/Manager.h"
#include "Reports/Manager.h"
#include "Reports/Common/StandardHeader.h"
#include "Translations/Translations.h"
#include "Utilities/CommandLineParser/CommandLineParser.h"
#include "Utilities/RandomNumberGenerator.h"
#include "Utilities/RunParameters.h"
#include "Logging/Logging.h"

// Namespaces
using namespace niwa;
using std::cout;
using std::endl;

#ifdef TESTMODE
#include <gtest/gtest.h>

int LoadOptions(int argc, char * argv[], niwa::utilities::RunParameters& options) { return -1; };
int PreParseConfigFiles(niwa::utilities::RunParameters& options) { return -1; };
int Run(int argc, char * argv[], niwa::utilities::RunParameters& options) { return -1; }
int RunUnitTests(int argc, char * argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  return result;
}
#else

/**
 * This method runs the unit tests
 */
int RunUnitTests(int argc, char * argv[]) {
  cout << "The shared library was built without TESTMODE enabled but it is trying to run unit tests..." << endl;
  return -1;
}

/**
 * This method loads the command-line arguments
 *
 * @return 0 if successful, -1 if not
 */
int LoadOptions(int argc, char * argv[], niwa::utilities::RunParameters& options) {
  try {
    niwa::utilities::CommandLineParser parser;
    parser.Parse(argc, argv, options);
  } catch (const string &exception) {
    cout << "## ERROR - Casal2 experienced a problem and has stopped execution" << endl;
    cout << "Error: " << exception << endl;
    return -1;
  } catch (std::exception& e) {
    cout << "## ERROR - Casal2 experienced a problem and has stopped execution" << endl;
    cout << e.what() << endl;
    return -1;
  } catch(...) {
    cout << "## ERROR - Casal2 experienced a problem and has stopped execution" << endl;
    cout << "The exception was caught with the catch-all. The type was unknown" << endl;
    cout << "Please contact the application developer" << endl;
    return -1;
  }

  return 0;
}

/**
 * This method is deprecated
 */
int PreParseConfigFiles(niwa::utilities::RunParameters& options) {
  LOG_CODE_ERROR() << "Code Deprecated";
  return -1;
}

/**
 * This is the main run method for the shared libraries. It is a modified version of main();
 */
int Run(int argc, char * argv[], niwa::utilities::RunParameters& options) {
  int return_code = 0;
  bool model_start_return_success = true;

  try {

    Model model;
    reports::StandardHeader standard_report(&model);

    model.global_configuration().set_run_parameters(options);
    RunMode::Type run_mode = options.run_mode_;

    vector<string> cmd_parameters;
    for (int i = 0; i < argc; ++i) cmd_parameters.push_back(argv[i]);
    model.global_configuration().set_command_line_parameters(cmd_parameters);

    /**
     * Check the run mode and call the handler.
     */
    switch (run_mode) {
    case RunMode::kInvalid:
    case RunMode::kUnitTest:
      LOG_ERROR() << "Invalid run mode specified.";
      break;

    case RunMode::kVersion:
    case RunMode::kLicense:
    case RunMode::kHelp:
      break;

    case RunMode::kQuery:
      {
        string lookup = model.global_configuration().object_to_query();
        vector<string> parts;
        boost::split(parts, lookup, boost::is_any_of("."));
        if (parts.size() == 1)
          parts.push_back("");
        if (parts.size() == 2) {
          model.set_partition_type(PartitionType::kAge);
          base::Object* object = model.factory().CreateObject(parts[0], parts[1], PartitionType::kModel);
          if (object) {
            cout << "Printing information for " << parts[0] << " with sub-type " << parts[1] << endl;
            object->PrintParameterQueryInfo();
          } else {
            model.set_partition_type(PartitionType::kLength);
            object = model.factory().CreateObject(parts[0], parts[1], PartitionType::kModel);
            if (object) {
              cout << "Printing information for " << parts[0] << " with sub-type " << parts[1] << endl;
              object->PrintParameterQueryInfo();
            } else
              cout << "Object type " << lookup << " is invalid" << endl;
          }
        } else {
          cout << "Please use object_type.sub_type only when querying an object" << endl;
        }
      }
      break;

    case RunMode::kTesting:
    case RunMode::kBasic:
    case RunMode::kEstimation:
    case RunMode::kMCMC:
    case RunMode::kSimulation:
    case RunMode::kProfiling:
    case RunMode::kProjection:
    {
      // Logging::Instance().SetLogLevel(options.log_level_);

      if (!model.global_configuration().debug_mode() && !model.global_configuration().disable_standard_report()) {
        standard_report.Prepare();
        model.managers().report()->set_std_header(standard_report.header());
      }

      // load our configuration file
      configuration::Loader config_loader(model);
      if (!config_loader.LoadConfigFile()) {
        Logging::Instance().FlushErrors();
        return_code = -1;
        break;
      }

      Logging& logging = Logging::Instance();
      config_loader.ParseFileLines();
      if (logging.errors().size() > 0) {
        logging.FlushErrors();
        return_code = -1;
        break;
      }

      // override any config file values from our command line
      model.global_configuration().ParseOptions(&model);
      utilities::RandomNumberGenerator::Instance().Reset(model.global_configuration().random_seed());

      // Thread off the reports
      reports::Manager* report_manager = model.managers().report();
      std::thread report_thread([&report_manager]() { report_manager->FlushReports(); });

      // Run the model
      model_start_return_success = model.Start(run_mode);

      // finish report thread
      report_manager->StopThread();
      report_thread.join();

      if (logging.errors().size() > 0) {
        logging.FlushErrors();
        return_code = -1;
      } else if (run_mode != RunMode::kTesting)
        logging.FlushWarnings();

      if (!model.global_configuration().debug_mode() && !model.global_configuration().disable_standard_report())
        standard_report.Finalise();
    }
      break;
    } // switch(run_mode)

    if (run_mode == RunMode::kTesting) {
      auto minimiser = model.managers().minimiser()->active_minimiser();
      if (minimiser)
        options.minimiser_ = minimiser->type();
    }

  } catch (const string &exception) {
    cout << "## ERROR - Casal2 experienced a problem and has stopped execution" << endl;
    cout << "Error: " << exception << endl;
    return_code = -1;

  } catch (std::exception& e) {
    cout << "## ERROR - Casal2 experienced a problem and has stopped execution" << endl;
    cout << e.what() << endl;
    return_code = -1;

  } catch(...) {
    cout << "## ERROR - Casal2 experienced a problem and has stopped execution" << endl;
    cout << "The exception was caught with the catch-all. The type was unknown" << endl;
    cout << "Please contact the application developer" << endl;
    return_code = -1;
  }

  if (!model_start_return_success) {
    LOG_FINEST() << "Done with errors";
    return -1;
  }

  LOG_FINEST() << "Done";
  return return_code;
}

#endif
