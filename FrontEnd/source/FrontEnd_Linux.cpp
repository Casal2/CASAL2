/**
 * @file FrontEnd_Linux.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 18, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science �2016 - www.niwa.co.nz
 *
 */
#ifdef __gnu_linux__
// headers
#include "FrontEnd.h"

#include <iostream>
#include <string>
#include <dlfcn.h>
#include <limits.h>
#include <unistd.h>

#include "Utilities/RunParameters.h"

// namespaces
using std::cout;
using std::endl;
using std::string;


typedef int(*RUNTESTSPROC)(int, char**);
typedef int(*RUNPROC)(int, char**, niwa::utilities::RunParameters&);
typedef int(*PRELOADPROC)(niwa::utilities::RunParameters&);
typedef int(*LOADOPTIONSPROC)(int, char**, niwa::utilities::RunParameters&);

string release_dll = "casal2_release.so";
string adolc_dll   = "casal2_adolc.so";
string betadiff_dll = "casal2_betadiff.so";
string cppad_dll = "casal2_cppad.so";
string test_dll  = "casal2_test.so";


std::string getexepath()
{
  char result[ PATH_MAX ];
  ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
  string temp = std::string( result, (count > 0) ? count : 0 );
  temp = temp.substr(0, temp.find_last_of("/")) + "/";
  return temp;
}

/**
 *
 */
int RunEstimationWithDll(int argc, char * argv[], niwa::utilities::RunParameters& options, const string& dll_name) {
  auto library = dlopen((getexepath() + dll_name).c_str(), RTLD_LAZY);
  if (library == nullptr) {
    cout << "Error: Failed to load CASAL2 Library: " << dll_name << endl;
    return -1;
  }

  auto proc = (RUNPROC)dlsym(library, "Run");
  return (proc)(argc, argv, options);
}

/**
 *
 */
int RunUnitTests(int argc, char * argv[]) {
  auto unit_test_library = dlopen((getexepath() + test_dll).c_str(), RTLD_LAZY);
  if (unit_test_library == nullptr) {
    cout << "Error: Failed to load CASAL2 Unit Test DLL: " << test_dll << endl;
    return -1;
  }

  auto unit_test_main_method =  (RUNTESTSPROC)dlsym(unit_test_library, "RunUnitTests");
  if (unit_test_main_method == nullptr) {
    cout << "Error: Failed to get the main method address" << endl;
    return -1;
  }

  (unit_test_main_method)(argc, argv);
  dlclose(unit_test_library);
  return 0;
}

/**
 *
 */
int run_for_os(int argc, char * argv[]) {
  for (int i = 0; i < argc; ++i) {
    if (string(argv[i]) == "--unittest") {
      cout << "Loading unit test DLL" << endl;
      return RunUnitTests(argc, argv);
    }
  }

  auto release_library = dlopen((getexepath() + release_dll).c_str(), RTLD_LAZY);
  if (release_library == NULL) {
    cout << "Error: Failed to load CASAL2 Release Library: " << getexepath() << release_dll << endl;
    return -1;
  }

  auto load_options = (LOADOPTIONSPROC)dlsym(release_library, "LoadOptions");
  if (load_options == nullptr) {
    cout << "Failed to get the LoadOptions method address" << endl;
    return -1;
  }
  int return_code = 0;
  niwa::utilities::RunParameters options;

  return_code = (load_options)(argc, argv, options);
  if (return_code != 0) {
	  dlclose(release_library);
    return return_code;
  }

  switch(options.run_mode_) {
  case RunMode::kLicense:
  case RunMode::kVersion:
  case RunMode::kHelp:
  case RunMode::kBasic:
  case RunMode::kMCMC:
  case RunMode::kSimulation:
  case RunMode::kProfiling:
  case RunMode::kProjection:
  case RunMode::kQuery:
  case RunMode::kTesting:
    {
      auto main_method = (RUNPROC)dlsym(release_library, "Run");
      if (main_method == nullptr) {
        cout << "Error: Failed to get the main method address" << endl;
        return_code = -1;
      } else {
        (main_method)(argc, argv, options);
      }
    }
    break;
  case RunMode::kEstimation:
    /**
     * 1. Pre-parse configuration file using release.dll
     * 2. Identify the active minimiser
     * 3. Pick DLL based on minimiser
     * 3. Default to release if no matching DLL was found
     */
    {
      auto preload_method = (PRELOADPROC)dlsym(release_library, "PreParseConfigFiles");
      if (preload_method == nullptr) {
        cout << "Error: Failed to get the preload method address" << endl;
        return_code = -1;
      } else {
        (preload_method)(options);
        if (options.minimiser_ == "cppad") {
          return_code = RunEstimationWithDll(argc, argv, options, cppad_dll);
        } else if (options.minimiser_ == "adolc") {
          return_code = RunEstimationWithDll(argc, argv, options, adolc_dll);
        } else if (options.minimiser_ == "betadiff") {
          return_code = RunEstimationWithDll(argc, argv, options, betadiff_dll);
        } else
          return_code = RunEstimationWithDll(argc, argv, options, release_dll);
      }
    }
    break;
  default:
    cout << "ERROR: Invalid RunMode:" << options.run_mode_ << endl;
    break;
  }

  dlclose(release_library);
	return return_code;
}
#endif
