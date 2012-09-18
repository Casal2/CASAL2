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
using namespace iSAM;
using std::cout;
using std::endl;

/**
 *
 */
int main(int argc, char * argv[]) {

  // Create instance now so it can record the time.
  Reports::StandardHeader stdReport;

  /**
   * Store our command line parameters
   */
  GlobalConfigurationPtr config = GlobalConfiguration::getInstance();
  for (int i = 0; i < argc; ++i)
    config->addCommandLineParameter(argv[i]);

  try {
    /**
     * Ask the runtime controller to parse the parameters.
     */
    RuntimeControllerPtr runtime = RuntimeController::getInstance();
    runtime->parseCommandLine(argc, (const char **)argv);

    /**
     * Check the run mode and call the handler.
     */
    switch (runtime->getRunMode()) {
    case RunMode::INVALID:
      THROW_EXCEPTION("Invalid run mode specified.");
      break;

    case RunMode::VERSION:
      cout << SOURCE_CONTROL_VERSION << endl;
      break;

    case RunMode::LICENSE:
      cout << "License has not been implemented yet" << endl;
      break;

    case RunMode::HELP:
      cout << runtime->getCommandLineUsage() << endl;
      break;

    default:
      stdReport.prepare();

      // runTime->run();

      stdReport.finalise();
      break;
    }

  } catch (const string &ex) {
    /**
     * This is the standard method of printing an error to the user. So
     * we expect exceptions to be thrown up cleanly.
     */
    cout << "## ERROR - iSAM has experience a problem and has stopped execution" << endl;
    cout << "## Execution stack trace: " << endl;

    // Un-Wind our Exception Stack
    int iLastLocation = 0;
    while (iLastLocation != -1) {
      int iLoc = ex.find_first_of(">", iLastLocation+1);
      cout << ex.substr(iLastLocation+1, (iLoc-iLastLocation)) << endl;
      iLastLocation = iLoc;
    }
    cout << endl;
    return -1;

  } catch(exception &ex) {
    /**
     * Unexpected exception was caught. We should be catching and handling all
     * exceptions internally and bubbling them up as const strings.
     */
    cout << "## ERROR - iSAM has encountered an unexpected exception" << endl;
    cout << "## Exception details: " << endl;
    cout << "## " << ex.what() << endl;
    return -1;
  }

	return 0;
}
#endif


