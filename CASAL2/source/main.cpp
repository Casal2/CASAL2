#ifdef TESTMODE
#include <gtest/gtest.h>

int main(int argc, char** argv) {
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
#include <string>

#include "Logging/Logging.h"
#include "SharedLibrary.h"

// Namespaces
using std::cout;
using std::endl;
using std::string;
using namespace niwa;

/**
 * Application entry point.
 * This code tries to match the FrontEnd binary as much as possible so that we can have
 * the same process for entering Casal2 whether it's stand alone or not.
 */
int main(int argc, char* argv[]) {
  int return_code = 0;

  try {
    // Step 1 - Parse the command line parameters
    utilities::RunParameters options;
    LoadOptions(argc, argv, options);

    // Step 2 - Pre-parse our configuration file
    // This isn't strictly necessary in stand alone, but it ensures
    // the configuration state is identical if we do it.
    RunMode::Type temp     = options.run_mode_;
    options.run_mode_      = RunMode::kTesting;
    options.no_std_report_ = true;
    string log_level       = options.log_level_;
    options.log_level_     = "error";

    return_code = PreParseConfigFiles(options);
    if (return_code != 0)
      return return_code;

    options.run_mode_      = temp;
    options.no_std_report_ = false;
    options.log_level_     = log_level;

    // Step 3 - Run the system through the SharedLibrary
    return_code = Run(argc, argv, options);

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
    cout << "Please contact the Casal2 development team and submit a bug report." << endl;
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
