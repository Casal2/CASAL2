#ifdef TESTMODE
#include <gtest/gtest.h>

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
#else

// defines
#ifndef BOOST_USE_WINDOWS_H
#define BOOST_USE_WINDOWS_H
#endif

// Headers
#include <iostream>
#include <thread>

#include "Version.h"
#include "ConfigurationLoader/Loader.h"
#include "GlobalConfiguration/GlobalConfiguration.h"
#include "Model/Managers.h"
#include "Model/Model.h"
#include "Reports/Children/StandardHeader.h"
#include "Reports/Manager.h"
#include "Utilities/CommandLineParser/CommandLineParser.h"
#include "Logging/Logging.h"

// Namespaces
using namespace niwa;
using std::cout;
using std::endl;

/**
 * Application entry point
 */
int main(int argc, char * argv[]) {
  Model model;
  reports::StandardHeader standard_report(&model);
  int return_code = 0;
  bool model_start_return_success = true;

  try {
    utilities::CommandLineParser parser(model, model.global_configuration());
    parser.Parse(argc, (const char **)argv);

    RunMode::Type run_mode = parser.run_mode();

    /**
     * Check the run mode and call the handler.
     */
    switch (run_mode) {
    case RunMode::kInvalid:
      LOG_ERROR() << "Invalid run mode specified.";
      break;

    case RunMode::kVersion:
      cout << SOURCE_CONTROL_VERSION << endl;
      break;

    case RunMode::kLicense:
      cout << "License has not been implemented yet" << endl;
      break;

    case RunMode::kHelp:
      cout << parser.command_line_usage() << endl;
      break;

    case RunMode::kQuery:
      break;

    default:
      if (!model.global_configuration().debug_mode() && !model.global_configuration().disable_standard_report())
        standard_report.Prepare();

      // load our configuration file
      configuration::Loader config_loader(model);
      if (!config_loader.LoadConfigFile()) {
        Logging::Instance().FlushErrors();
        return_code = -1;
        break;
      }

      // override any config file values from our command line
      model.global_configuration().OverrideGlobalValues(parser.override_values());

      // Thread off the reports
      reports::Manager* report_manager = model.managers().report();
      std::thread report_thread([report_manager]() { report_manager->FlushReports(); });

      // Run the model
      model_start_return_success = model.Start(run_mode);

      // finish report thread
      report_manager->StopThread();
      report_thread.join();

      Logging& logging = Logging::Instance();
      if (logging.errors().size() > 0) {
        logging.FlushErrors();
        return_code = -1;
      }

      logging.FlushWarnings();

      if (!model.global_configuration().debug_mode() && !model.global_configuration().disable_standard_report())
        standard_report.Finalise();
      break;
    } // switch(run_mode)

  } catch (const string &exception) {
    cout << "## ERROR - iSAM experienced a problem and has stopped execution" << endl;
    cout << "Error: " << exception << endl;
    return_code = -1;

  } catch (std::exception& e) {
    cout << "## ERROR - iSAM experienced a problem and has stopped execution" << endl;
    cout << e.what() << endl;
    return_code = -1;

  } catch(...) {
    cout << "## ERROR - iSAM experienced a problem and has stopped execution" << endl;
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


