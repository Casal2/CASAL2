"""
This module contains the list of global variables we're going to be
using throughout the build system
"""
operating_system_ = ""
path_             = ""
compiler_path_    = ""
gfortran_path_    = ""
compiler_version_ = ""
latex_path_       = ""
git_path_         = ""
cmd_path_         = ""
build_target_     = ""
build_parameters_ = ""
target_success_path_     = ""
target_include_path_     = ""
target_debug_lib_path_   = ""
target_release_lib_path_ = ""
target_special_lig_path_ = ""

allowed_build_targets_ = [ "debug", "release", "documentation", "thirdparty", "thirdpartylean",
                           "test", "archive", "all", "clean", "cleanall", "help",
                           "check", "modelrunner", "installer" ]
allowed_build_types_ = [ "debug", "release", "test" ]
allowed_build_parameters_ = [ "", "adolc", "betadiff", "cppad" ]

def PrintError(error_message):
  print "\n\n\n"
  print "###### ERROR ######"
  print "Error Description:"
  print error_message
  print "###### ERROR ######"
  print "\n\n\n"
  return False
