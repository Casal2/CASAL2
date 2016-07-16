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
#include "Reports/Children/StandardHeader.h"
#include "Reports/Manager.h"
#include "Translations/Translations.h"
#include "Utilities/CommandLineParser/CommandLineParser.h"
#include "Utilities/RandomNumberGenerator.h"
#include "Utilities/RunParameters.h"
#include "Logging/Logging.h"

#include "Utilities/CommandLineParser/CommandLineParser.h"

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
 *
 */
int RunUnitTests(int argc, char * argv[]) {
  cout << "DLL was built without TESTMODE enabled but it's trying to run unit tests..." << endl;
  return -1;
}

/**
 *
 */
int LoadOptions(int argc, char * argv[], niwa::utilities::RunParameters& options) {
  try {
    niwa::utilities::CommandLineParser parser;
    parser.Parse(argc, argv, options);
  } catch (const string &exception) {
    cout << "## ERROR - CASAL2 experienced a problem and has stopped execution" << endl;
    cout << "Error: " << exception << endl;
    return -1;
  } catch (std::exception& e) {
    cout << "## ERROR - CASAL2 experienced a problem and has stopped execution" << endl;
    cout << e.what() << endl;
    return -1;
  } catch(...) {
    cout << "## ERROR - CASAL2 experienced a problem and has stopped execution" << endl;
    cout << "The exception was caught with the catch-all. The type was unknown" << endl;
    cout << "Please contact the application developer" << endl;
    return -1;
  }

  return 0;
}

/**
 *
 */
int PreParseConfigFiles(niwa::utilities::RunParameters& options) {
  // load our configuration file
  Model model;
  model.global_configuration().set_run_parameters(options);

  configuration::Loader config_loader(model);
  if (!config_loader.LoadConfigFile()) {
    Logging::Instance().FlushErrors();
    return -1;
  }

  // Find the defined minimisers
  auto file_lines = config_loader.file_lines();
  vector<unsigned> minimiser_indexes;
  for (unsigned i = 0; i < file_lines.size(); ++i) {
    auto file_line = file_lines[i];
    if (file_line.line_.substr(0, 10) == "@minimiser") {
      minimiser_indexes.push_back(i);
    }
  }

  // Find the active minimisers
  vector<unsigned> active_minimiser_indexes;
  if (minimiser_indexes.size() == 1)
    active_minimiser_indexes.push_back(minimiser_indexes[0]);
  else {
    for (unsigned index : minimiser_indexes) {
      unsigned offset = 0;
      string line = "";
      while (line.substr(0,1) != "@") {
        ++offset;
        line = file_lines[index + offset].line_;
        if (line.substr(0, 6) == "active") {
          vector<string> holding_vec;
          boost::split(holding_vec, line, boost::is_any_of(" "));
          if (holding_vec.size() != 2) {
            LOG_FATAL() << "Holding Vector size was " << holding_vec.size() << " when determining active value" << endl;
            return -1;
          }

          bool active = false;
          if (!utilities::To<bool>(holding_vec[1], active)) {
            LOG_FATAL() << holding_vec[1] << " could not be converted to a boolean to check if minimiser was active" << endl;
            return -1;
          }

          if (active)
            active_minimiser_indexes.push_back(index);
        }
      }
    }
  }

  if (active_minimiser_indexes.size() == 0) {
    LOG_FATAL() << "No active @minimiser blocks have been found in the configuration files";
  } else if (active_minimiser_indexes.size() > 1) {
    LOG_FATAL() << active_minimiser_indexes.size() << " active @minimiser's were found in the configuration files";
  }

  unsigned offset = 0;
  string line = "";
  while (line.substr(0, 1) != "@") {
    ++offset;
    line = file_lines[active_minimiser_indexes[0] + offset].line_;
    if (line.substr(0, 4) == "type") {
      vector<string> holding_vec;
      boost::split(holding_vec, line, boost::is_any_of(" "));
      if (holding_vec.size() != 2) {
        LOG_FATAL() << "Got " << holding_vec.size() << " values instead of 2 when splitting minimiser type " << line << " by space";
      }
      options.minimiser_ = holding_vec[1];
    }
  }

  return 0;
}

/**
 * This is the main run method for our DLL. It's a modified version of main();
 */
int Run(int argc, char * argv[], niwa::utilities::RunParameters& options) {
  int return_code = 0;
  bool model_start_return_success = true;

  try {
    Model model;
    model.global_configuration().set_run_parameters(options);
    RunMode::Type run_mode = options.run_mode_;

    reports::StandardHeader standard_report(&model);

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
          base::Object* object = model.factory().CreateObject(parts[0], parts[1]);
          if (object) {
            cout << "Printing information for " << parts[0] << " with sub-type " << parts[1] << endl;
            object->PrintParameterQueryInfo();
          } else {
            cout << "Object type " << lookup << " is invalid" << endl;
          }
        } else {
          cout << "Please use object_type.sub_type only when querying an object" << endl;
        }
      }
      break;

    case RunMode::kBasic:
    case RunMode::kEstimation:
    case RunMode::kMCMC:
    case RunMode::kSimulation:
    case RunMode::kProfiling:
    case RunMode::kProjection:
    case RunMode::kTesting:
    {
      if (!model.global_configuration().debug_mode() && !model.global_configuration().disable_standard_report())
        standard_report.Prepare();

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
      }

      logging.FlushWarnings();

      if (!model.global_configuration().debug_mode() && !model.global_configuration().disable_standard_report())
        standard_report.Finalise();
    }
      break;
    } // switch(run_mode)

  } catch (const string &exception) {
    cout << "## ERROR - CASAL2 experienced a problem and has stopped execution" << endl;
    cout << "Error: " << exception << endl;
    return_code = -1;

  } catch (std::exception& e) {
    cout << "## ERROR - CASAL2 experienced a problem and has stopped execution" << endl;
    cout << e.what() << endl;
    return_code = -1;

  } catch(...) {
    cout << "## ERROR - CASAL2 experienced a problem and has stopped execution" << endl;
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
