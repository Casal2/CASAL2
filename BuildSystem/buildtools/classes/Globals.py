"""
This module contains the list of global variables we're going to be
using throughout the build system
"""
import os
Casal2_version_number = "1.2" # This defines the VERSION of Casal2 throughout all code. 
operating_system_     = ""
path_                 = ""
compiler_path_        = ""
gfortran_path_        = ""
python_cmd_           = ""
compiler_ 		      = ""
compiler_version_     = ""
make_command_         = ""
cmake_compiler_	      = "unknown"
latex_path_           = ""
git_path_             = ""
cmd_path_             = ""
build_target_         = ""
build_parameters_     = ""
target_success_path_      = ""
target_include_path_      = ""
target_debug_lib_path_    = ""
target_release_lib_path_  = ""
target_special_lib_path_  = ""
threads_                  = 0 # This is taken from CASAL2_BUILD_THREADS environment variable

allowed_build_targets_ = [ "debug", "release", "documentation", "thirdparty", "thirdpartylean",
                           "test", "archive", "all", "clean", "clean_cache", "clean_all", "help", "library",
                           "check", "modelrunner", "installer", "deb", "rlibrary", "frontend",
                           "version" ]
allowed_build_types_ = [ "debug", "release", "test" ]
allowed_build_parameters_ = [ "", "adolc", "betadiff" ]
allowed_library_parameters_ = [ "release", "test", "adolc", "betadiff" ]

EX_OK = getattr(os, "EX_OK", 0)

def PrintError(error_message):
  print("\n\n\n")
  print("###### ERROR ######")
  print( "Error Description:")
  print(error_message)
  print("###### ERROR ######")
  print("\n\n\n")
  return False
