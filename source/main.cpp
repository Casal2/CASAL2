#ifdef TEST
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_MODULE SPM_Unit_Tests

// Headers
#include <boost/test/included/unit_test.hpp>
#else

// Headers
#include <iostream>

#include "Version.h"
#include "GlobalConfiguration/GlobalConfiguration.h"
#include "Reports/Children/StandardHeader.h"
#include "RuntimeController/RuntimeController.h"

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
    RuntimeControllerPtr runtime = RuntimeController::Instance();
    runtime->ParseCommandLine(argc, (const char **)argv);

    /**
     * Check the run mode and call the handler.
     */
    switch (runtime->run_mode()) {
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
      cout << runtime->command_line_usage() << endl;
      break;

    default:
      standard_report.Prepare();

      // runTime->run();

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

  } catch(exception &isam_exception) {
    /**
     * Unexpected exception was caught. We should be catching and handling all
     * exceptions internally and bubbling them up as const strings.
     */
    cout << "## ERROR - iSAM has encountered an unexpected exception" << endl;
    cout << "## Exception details: " << endl;
    cout << "## " << isam_exception.what() << endl;
    return -1;
  }

	return 0;
}
#endif


