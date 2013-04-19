"""
This module contains the list of global variables we're going to be
using throughout the build system
"""
operating_system_ = ""
path_             = ""
compiler_path_    = ""
cmd_path_         = ""
build_target_     = ""
build_parameters_ = ""
target_path_      = ""

def PrintError(error_message):
  print "\n\n\n"
  print "###### ERROR ######"
  print "Error Description:"
  print error_message
  print "###### ERROR ######"
  print "\n\n\n"
  return False