/**
 * @file FrontEnd.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 13, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 */

// headers
#include <iostream>
#include <string>
#include <windows.h>

#include "Utilities/RunParameters.h"

// namespaces
using std::cout;
using std::endl;
using std::string;

typedef int(__cdecl *RUNTESTSPROC)(int, char**);
typedef int(__cdecl *RUNPROC)(int, char**, niwa::utilities::RunParameters&);
typedef int(__cdecl *PRELOADPROC)(niwa::utilities::RunParameters&);
typedef int(__cdecl *LOADOPTIONSPROC)(int, char**, niwa::utilities::RunParameters&);

/**
 *
 */
int RunEstimationWithDll(int argc, char * argv[], niwa::utilities::RunParameters& options, const string& dll_name = "") {
  string full_dll_name = "../../BuildSystem/bin/windows/library_" + dll_name + "/libcasal2.dll";

  auto library = LoadLibrary(full_dll_name.c_str());
  if (library == nullptr) {
    cout << "Error: Failed to load CASAL2 Library: " << full_dll_name << endl;
    return -1;
  }

  auto proc = (RUNPROC)GetProcAddress(library, "Run");
  return (proc)(argc, argv, options);
}

/**
 *
 */
int RunUnitTests(int argc, char * argv[]) {
  auto unit_test_library = LoadLibrary("../../BuildSystem/bin/windows/library_test/libcasal2.dll");
  if (unit_test_library == nullptr) {
    cout << "Error: Failed to load CASAL2 Unit Test DLL" << endl;
    return -1;
  }

  auto unit_test_main_method =  (RUNTESTSPROC)GetProcAddress(unit_test_library, "RunUnitTests");
  if (unit_test_main_method == nullptr) {
    cout << "Error: Failed to get the main method address" << endl;
    return -1;
  }

  (unit_test_main_method)(argc, argv);
  FreeLibrary(unit_test_library);
  return 0;
}

/**
 *
 */
int main(int argc, char * argv[]) {
  for (int i = 0; i < argc; ++i) {
    if (string(argv[i]) == "--unittest") {
      cout << "Loading unit test DLL" << endl;
      return RunUnitTests(argc, argv);
    }
  }

  auto release_library = LoadLibrary("../../BuildSystem/bin/windows/library_release/libcasal2.dll");
  if (release_library == nullptr) {
    cout << "Error: Failed to load CASAL2 Release Library" << endl;
    return -1;
  }

  auto load_options = (LOADOPTIONSPROC)GetProcAddress(release_library, "LoadOptions");
  if (load_options == nullptr) {
    cout << "Failed to get the LoadOptions method address" << endl;
    return -1;
  }
  int return_code = 0;
  niwa::utilities::RunParameters options;

  return_code = (load_options)(argc, argv, options);
  if (return_code != 0) {
    FreeLibrary(release_library);
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
      auto main_method = (RUNPROC)GetProcAddress(release_library, "Run");
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
      auto preload_method = (PRELOADPROC)GetProcAddress(release_library, "PreParseConfigFiles");
      if (preload_method == nullptr) {
        cout << "Error: Failed to get the preload method address" << endl;
        return_code = -1;
      } else {
        (preload_method)(options);
        if (options.minimiser_ == "cppad") {
          return_code = RunEstimationWithDll(argc, argv, options, "cppad");
        } else if (options.minimiser_ == "adolc") {
          return_code = RunEstimationWithDll(argc, argv, options, "adolc");
        } else if (options.minimiser_ == "betadiff") {
          return_code = RunEstimationWithDll(argc, argv, options, "betadiff");
        } else
          return_code = RunEstimationWithDll(argc, argv, options, "release");
      }
    }
    break;
  default:
    cout << "ERROR: Invalid RunMode:" << options.run_mode_ << endl;
    break;
  }

  FreeLibrary(release_library);
	return return_code;
}


