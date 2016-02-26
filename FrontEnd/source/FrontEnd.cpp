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
typedef int(__cdecl *RUNTESTSPROC)(int, char**);
typedef int(__cdecl *RUNPROC)(int, char**, niwa::utilities::RunParameters&);
typedef int(__cdecl *PRELOADPROC)(niwa::utilities::RunParameters&);
typedef int(__cdecl *LOADOPTIONSPROC)(int, char**, niwa::utilities::RunParameters&);

const string release_lib  = "casal2_release.dll";
const string adolc_lib    = "casal2_adolc.dll";
const string betadiff_lib = "casal2_betadiff.dll";
const string cppad_lib    = "casal2_cppad.dll";
const string test_lib     = "casal2_test.dll";
#else
typedef int(*RUNTESTSPROC)(int, char**);
typedef int(*RUNPROC)(int, char**, niwa::utilities::RunParameters&);
typedef int(*PRELOADPROC)(niwa::utilities::RunParameters&);
typedef int(*LOADOPTIONSPROC)(int, char**, niwa::utilities::RunParameters&);

string release_lib  = "/usr/local/lib/casal2_release.so";
string adolc_lib    = "/usr/local/lib/casal2_adolc.so";
string betadiff_lib = "/usr/local/lib/casal2_betadiff.so";
string cppad_lib    = "/usr/local/lib/casal2_cppad.so";
string test_lib     = "/usr/local/lib/casal2_test.so";
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
void* LoadSharedLibrary(const string& name) {
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
void RunBasic(int argc, char * argv[], niwa::utilities::RunParameters &options);
void RunEstimation(int argc, char * argv[], niwa::utilities::RunParameters &options);
void RunMCMC(int argc, char * argv[], niwa::utilities::RunParameters &options);
void RunUnitTests(int argc, char * argv[], niwa::utilities::RunParameters &options);

// Local Variables
int return_code_ = 0;
/**
 *
 */
int main(int argc, char * argv[]) {
  niwa::utilities::RunParameters options;
  /*
   * load our release library to parse the command line
   * parameters
   */
  auto release_library = LoadSharedLibrary(release_lib.c_str());
  if (release_library == nullptr) {
    cout << "Error: Failed to load CASAL2 Release Library: " << release_lib << endl;
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

  /**
   * Pre-Parse our configuration file so we can determine what library we want
   * to use for the estimation
   */
  auto preload_method = (PRELOADPROC)LoadLibraryFunction(release_library, "PreParseConfigFiles");
  if (preload_method == nullptr) {
    cout << "Error: Failed to get the PreParseConfigFiles method address" << endl;
    CloseLibrary(release_library);
    return -1;
  }
  if ((preload_method)(options) != 0) {
    return -1;
  }

  switch(options.run_mode_) {
  case RunMode::kLicense:
  case RunMode::kVersion:
  case RunMode::kHelp:
  case RunMode::kBasic:
  case RunMode::kSimulation:
  case RunMode::kProfiling:
  case RunMode::kProjection:
  case RunMode::kQuery:
  case RunMode::kTesting:
    RunBasic(argc, argv, options);
    break;
  case RunMode::kEstimation:
    RunEstimation(argc, argv, options);
    break;
  case RunMode::kMCMC:
    RunMCMC(argc, argv, options);
    break;
  case RunMode::kUnitTest:
    RunUnitTests(argc, argv, options);
    break;
  default:
    cout << "ERROR: Invalid RunMode:" << options.run_mode_ << endl;
    break;
  }

  CloseLibrary(release_library);
	return return_code_;
}

/**
 * Do a basic run of our system using the release
 * library
 */
void RunBasic(int argc, char * argv[], niwa::utilities::RunParameters &options) {
  /*
   * load our release library to parse the command line
   * parameters
   */
  auto release_library = LoadSharedLibrary(release_lib.c_str());
  if (release_library == nullptr) {
    cout << "Error: Failed to load CASAL2 Release Library: " << release_lib << endl;
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
void RunEstimation(int argc, char * argv[], niwa::utilities::RunParameters &options) {
  string library_name = release_lib;
  if (options.minimiser_ == "cppad")
    library_name = cppad_lib;
  else if (options.minimiser_ == "adolc")
    library_name = adolc_lib;
  else if (options.minimiser_ == "betadiff")
    library_name = betadiff_lib;

  auto library = LoadSharedLibrary(library_name);
  if (library == nullptr) {
    cout << "Error: Failed to load CASAL2 Library: " << library_name << endl;
    CloseLibrary(library);
    return_code_ = -1;
    return;
  }

  auto proc = (RUNPROC)LoadLibraryFunction(library, "Run");
  return_code_ =  (proc)(argc, argv, options);
  CloseLibrary(library);
}

/**
 * Run our MCMC. But first we want to run an Estimation
 */
void RunMCMC(int argc, char * argv[], niwa::utilities::RunParameters &options) {
  options.create_mpd_file_ = true;
  options.no_std_report_   = true;
  options.run_mode_        = RunMode::kEstimation;
  RunEstimation(argc, argv, options);
  if (return_code_ != 0)
    return;

  options.run_mode_        = RunMode::kMCMC;
  RunBasic(argc, argv, options);
}

/**
 *
 */
void RunUnitTests(int argc, char * argv[], niwa::utilities::RunParameters &options) {
  auto unit_test_library = LoadSharedLibrary(test_lib.c_str());
  if (unit_test_library == nullptr) {
    cout << "Error: Failed to load CASAL2 Unit Test Library: " << test_lib << endl;
    return_code_ = -1;
    return;
  }

  auto unit_test_main_method =  (RUNTESTSPROC)LoadLibraryFunction(unit_test_library, "RunUnitTests");
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
