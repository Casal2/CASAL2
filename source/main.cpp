#ifdef TEST
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_MODULE SPM_Unit_Tests

// Headers
#include <boost/test/included/unit_test.hpp>
#else

// Headers
#include <iostream>

#include "Version.h"
#include "ConfigurationLoader/Loader.h"
#include "GlobalConfiguration/GlobalConfiguration.h"
#include "Model/Model.h"
#include "Reports/Children/StandardHeader.h"
#include "Utilities/CommandLineParser/CommandLineParser.h"

// Namespaces
using namespace isam;
using std::cout;
using std::endl;

/**
 * Application entry point
 */
int main(int argc, char * argv[]) {

  // Create instance now so it can record the time.
  reports::StandardHeader standard_report;

  /**
   * Store our command line parameters
   */
  GlobalConfigurationPtr config = GlobalConfiguration::Instance();

  vector<string> parameters;
  for (int i = 0; i < argc; ++i)
    parameters.push_back(argv[i]);
  config->set_command_line_parameters(parameters);

  try {
    /**
     * Ask the runtime controller to parse the parameters.
     */
    isam::utilities::CommandLineParser parser;
    parser.Parse(argc, (const char **)argv);

    /**
     * Check the run mode and call the handler.
     */
    ModelPtr model = Model::Instance();
    switch (model->run_mode()) {
    case RunMode::kInvalid:
      THROW_EXCEPTION("Invalid run mode specified.");
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

    default:
      if (!config->debug_mode())
        standard_report.Prepare();

      /**
       * Load our configuration files
       */
      configuration::Loader config_loader;
      config_loader.LoadConfigFile();

      /**
       * Override any config values
       */
      parser.OverrideGlobalValues();

      // Run the model
      model->Start();

      if (!config->debug_mode())
        standard_report.Finalise();
      break;
    }

  } catch (const string &isam_exception) {
    /**
     * This is the standard method of printing an error to the user. So
     * we expect exceptions to be thrown up cleanly.
     */
    cout << "## ERROR - iSAM experienced a problem and has stopped execution" << endl;
    cout << "## Execution stack: " << endl;

    // Un-Wind our Exception Stack
    int last_location = 0;
    while (last_location != -1) {
      int location = isam_exception.find_first_of(">", last_location+1);
      cout << isam_exception.substr(last_location+1, (location-last_location)) << endl;
      last_location = location;
    }
    cout << endl;
    return -1;

  }

	return 0;
}
#endif


