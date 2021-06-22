/**
 * @file FrontEnd.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 13, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science �2016 - www.niwa.co.nz
 *
 */

// headers
#include <iostream>
#include <string>

#ifdef __MINGW32__
#include <windows.h>
#else
#include <dlfcn.h>
#include <limits.h>
#include <unistd.h>
#endif

#include "Utilities/RunParameters.h"

// namespaces
using std::cout;
using std::endl;
using std::string;

#ifdef __MINGW32__
typedef int(__cdecl* RUNTESTSPROC)(int, char**);
typedef int(__cdecl* RUNPROC)(int, char**, niwa::utilities::RunParameters&);
typedef int(__cdecl* PRELOADPROC)(niwa::utilities::RunParameters&);
typedef int(__cdecl* LOADOPTIONSPROC)(int, char**, niwa::utilities::RunParameters&);

const string release_lib  = "casal2_release.dll";
const string adolc_lib    = "casal2_adolc.dll";
const string betadiff_lib = "casal2_betadiff.dll";
// const string cppad_lib    = "casal2_cppad.dll";
const string test_lib = "casal2_test.dll";
#else
typedef int (*RUNTESTSPROC)(int, char**);
typedef int (*RUNPROC)(int, char**, niwa::utilities::RunParameters&);
typedef int (*PRELOADPROC)(niwa::utilities::RunParameters&);
typedef int (*LOADOPTIONSPROC)(int, char**, niwa::utilities::RunParameters&);

string release_lib  = "/usr/local/lib/casal2_release.so";
string adolc_lib    = "/usr/local/lib/casal2_adolc.so";
string betadiff_lib = "/usr/local/lib/casal2_betadiff.so";
// string cppad_lib    = "/usr/local/lib/casal2_cppad.so";
string test_lib = "/usr/local/lib/casal2_test.so";
#endif

#ifdef __MINGW32__
HMODULE LoadSharedLibrary(const string& name) {
  return LoadLibrary(name.c_str());
}

FARPROC LoadLibraryFunction(HMODULE library, string method) {
  return GetProcAddress(library, method.c_str());
}

void CloseLibrary(HMODULE library) {
  FreeLibrary(library);
}

#else
void*  LoadSharedLibrary(const string& name) {
  return dlopen(name.c_str(), RTLD_LAZY);
}

void* LoadLibraryFunction(void* library, string method) {
  return dlsym(library, method.c_str());
}

void CloseLibrary(void* library) {
  dlclose(library);
}
#endif

// Function Prototypes
void RunConfigurationFileValidation(int argc, char* argv[], niwa::utilities::RunParameters& options);
void RunBasic(int argc, char* argv[], niwa::utilities::RunParameters& options);
void RunEstimation(int argc, char* argv[], niwa::utilities::RunParameters& options);
void RunMCMC(int argc, char* argv[], niwa::utilities::RunParameters& options);
void RunUnitTests(int argc, char* argv[], niwa::utilities::RunParameters& options);

// Local Variables
int return_code_ = 0;
/**
 *
 */
int main(int argc, char* argv[]) {
  niwa::utilities::RunParameters options;

  /*
   * load our release library to parse the command line
   * parameters
   */
  auto release_library = LoadSharedLibrary(release_lib.c_str());
  if (release_library == nullptr) {
    cout << "Error: Failed to load Casal2 Release Library: " << release_lib << endl;
    return -1;
  }

  /**
   * Load the "LoadOptions" method and parse our options
   */
  auto load_options = (LOADOPTIONSPROC)LoadLibraryFunction(release_library, "LoadOptions");
  if (load_options == nullptr) {
    CloseLibrary(release_library);
    cout << "Failed to get the LoadOptions method address" << endl;
    return -1;
  }

  /**
   * Run the LoadOptions method to parse the
   * command line parameters
   */
  return_code_ = (load_options)(argc, argv, options);
  if (return_code_ != 0) {
    CloseLibrary(release_library);
    return return_code_;
  }

  switch (options.run_mode_) {
    case RunMode::kLicense:
    case RunMode::kVersion:
    case RunMode::kHelp:
      return 0;
    case RunMode::kUnitTest:
      RunUnitTests(argc, argv, options);
      return return_code_;
    case RunMode::kQuery:
      RunBasic(argc, argv, options);
      return return_code_;
    default:
      break;
  }

  /**
   * Pre-Validate our configuration file and determine what minimiser
   * to use for the estimation
   */

  RunConfigurationFileValidation(argc, argv, options);
  if (return_code_ != 0)
    return return_code_;

  // go go go
  switch (options.run_mode_) {
    case RunMode::kLicense:
    case RunMode::kVersion:
    case RunMode::kHelp:
    case RunMode::kBasic:
    case RunMode::kSimulation:
    case RunMode::kProfiling:
    case RunMode::kProjection:
    // case RunMode::kQuery:
    case RunMode::kTesting:
      RunBasic(argc, argv, options);
      break;
    case RunMode::kEstimation:
      RunEstimation(argc, argv, options);
      break;
    case RunMode::kMCMC:
      RunMCMC(argc, argv, options);
      break;
    default:
      cout << "ERROR: Invalid RunMode:" << options.run_mode_ << endl;
      break;
  }

  CloseLibrary(release_library);
  return return_code_;
}

/**
 * This method will re-load the release library to parse the configuration file for any
 * validation errors. We also use this to figure out what minimisers or MCMC objects we want
 * to use so we can load proper files in the future.
 */
void RunConfigurationFileValidation(int argc, char* argv[], niwa::utilities::RunParameters& options) {
  /*
   * load our release library to parse the command line
   * parameters
   */
  auto release_library = LoadSharedLibrary(release_lib.c_str());
  if (release_library == nullptr) {
    cout << "Error: Failed to load Casal2 Release Library: " << release_lib << endl;
    return_code_ = -1;
    return;
  }

  /**
   * Load the "LoadOptions" method and parse our options
   */
  auto main_method = (PRELOADPROC)LoadLibraryFunction(release_library, "PreParseConfigFiles");
  if (main_method == nullptr) {
    cout << "Error: Failed to get the main method address" << endl;
    CloseLibrary(release_library);
    return_code_ = -1;
    return;
  }

  RunMode::Type temp     = options.run_mode_;
  options.run_mode_      = RunMode::kTesting;
  options.no_std_report_ = true;
  string log_level       = options.log_level_;
  options.log_level_     = "error";

  return_code_ = (main_method)(options);

  options.run_mode_      = temp;
  options.no_std_report_ = false;
  options.log_level_     = log_level;

  return;
}

/**
 * Do a basic run of our system using the release
 * library
 */
void RunBasic(int argc, char* argv[], niwa::utilities::RunParameters& options) {
  /*
   * load our release library to parse the command line
   * parameters
   */
  auto release_library = LoadSharedLibrary(release_lib.c_str());
  if (release_library == nullptr) {
    cout << "Error: Failed to load Casal2 Release Library: " << release_lib << endl;
    return_code_ = -1;
    return;
  }

  /**
   * Load the "LoadOptions" method and parse our options
   */
  auto main_method = (RUNPROC)LoadLibraryFunction(release_library, "Run");
  if (main_method == nullptr) {
    cout << "Error: Failed to get the main method address" << endl;
    CloseLibrary(release_library);
    return_code_ = -1;
    return;
  }

  return_code_ = (main_method)(argc, argv, options);
}

/**
 * Do An estimation run of our model
 */
void RunEstimation(int argc, char* argv[], niwa::utilities::RunParameters& options) {
  string library_name = release_lib;
  // if (options.minimiser_ == "cppad")
  // library_name = cppad_lib;
  if (options.minimiser_ == "adolc")
    library_name = adolc_lib;
  else if (options.minimiser_ == "betadiff")
    library_name = betadiff_lib;

  auto minimiser_library = LoadSharedLibrary(library_name);
  if (minimiser_library == nullptr) {
    cout << "Error: Failed to load Casal2 Library: " << library_name << endl;
    return_code_ = -1;
    return;
  }

  auto proc    = (RUNPROC)LoadLibraryFunction(minimiser_library, "Run");
  return_code_ = (proc)(argc, argv, options);
  CloseLibrary(minimiser_library);
}

/**
 * Run our MCMC. But first we want to run an Estimation.
 *
 * This is different than the standalone executable. The standalone
 * executable will handle the minimisation itself. With the front end application
 * we want to handle it so we can load specific DLLs/SOs with different autodiff
 * minimisers, then swap back to the release.dll/.so for MCMC so it's not hideously
 * slow.
 */
void RunMCMC(int argc, char* argv[], niwa::utilities::RunParameters& options) {
  bool printed_header = false;
  if (options.estimate_before_mcmc_) {
    options.disable_all_reports_     = true;   // otherwise they print at end of execution
    options.estimation_is_for_mcmc_  = true;   // we can validate we'll have an MPD
    options.print_std_report_header_ = true;   // Print start of Casal2 Std header
    options.print_std_report_footer_ = false;  // Don't do the time calculation and print that
    options.run_mode_                = RunMode::kEstimation;
    printed_header                   = true;
    RunEstimation(argc, argv, options);
    if (return_code_ != 0)
      return;
  }

  options.disable_all_reports_     = false;
  options.print_std_report_header_ = !printed_header;  // Don't print Casal2 std header to console
  options.print_std_report_footer_ = true;             // Do the time calculation and print that
  options.estimate_before_mcmc_    = false;            // Stop the model from re-estimating
  options.run_mode_                = RunMode::kMCMC;
  RunBasic(argc, argv, options);
}

/**
 *
 */
void RunUnitTests(int argc, char* argv[], niwa::utilities::RunParameters& options) {
  auto unit_test_library = LoadSharedLibrary(test_lib.c_str());
  if (unit_test_library == nullptr) {
    cout << "Error: Failed to load Casal2 Unit Test Library: " << test_lib << endl;
    return_code_ = -1;
    return;
  }

  auto unit_test_main_method = (RUNTESTSPROC)LoadLibraryFunction(unit_test_library, "RunUnitTests");
  if (unit_test_main_method == nullptr) {
    cout << "Error: Failed to get the main method address" << endl;
    return_code_ = -1;
    CloseLibrary(unit_test_library);
    return;
  }

  return_code_ = (unit_test_main_method)(argc, argv);
  CloseLibrary(unit_test_library);
  return;
}
