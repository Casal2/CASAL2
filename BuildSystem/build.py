import os
import sys
import os.path
import subprocess
import fileinput

sys.path.insert(0, "buildtools/classes")

# These are iSAM specific python objects
from System import *
from Globals import *
from Builder import *
from Documentation import *
from Archiver import *
from ModelRunner import *

"""
Print the usage for this build system
"""
def print_usage():  
  os.system( [ 'clear', 'cls' ][ os.name == 'nt' ] )
  print '###########################################################'
  print '# iSAM Build System Usage                                 #'
  print '###########################################################'
  print 'Usage:'
  print 'doBuild <build_target> <build_parameter>'
  print ''
  print 'Valid Build Types:'
  print '  debug - Build debug version'
  print '  release - Build release version'
  print '  test - Build unit tests'
  print '  documentation - Build the user manual'
  print '  thirdparty - Build all required third party libraries'
  print '  all - Does clean, thirdparty, debug, release builds in order'
  print '  clean - Remove any previous debug/release build information'
  print '  cleanall - Remove all previous build information'
  print '  archive - Build a zipped archive of the application'
  print '  check - Do a check of the build system'
  print '  modelrunner - Run the test suite of models'
  print ''
  print 'Valid Build Parameters: (thirdparty only)'
  print '  <libary name> - Target third party library to build or rebuild'
  print ''
  print 'Valid Build parameters: (debug/release only)'
  print '  adolc - Use ADOLC auto-differentiation in compiled executable'
  print '  betadiff - Use BetaDiff auto-differentiation (from CASAL)'
  print '  cppad - Use CppAD auto-differentiation'
  return True


"""
Start our build system. This method essentially just
gets all of the system information we need and will
test that executables are in the path that we need
"""
def start_build_system():
  if Globals.operating_system_ == "win32":
    Globals.cmd_path_      = system_info.find_exe_path('cmd.exe')
    Globals.compiler_path_ = system_info.find_exe_path('g++.exe')
    Globals.gfortran_path_ = system_info.find_exe_path('gfortran.exe')
    Globals.latex_path_    = system_info.find_exe_path('bibtex.exe')
    Globals.git_path_      = system_info.find_exe_path('git.exe')
  else:
    Globals.compiler_path_ = system_info.find_exe_path('g++')
    Globals.gfortran_path_ = system_info.find_exe_path('gfortran')
    Globals.latex_path_    = system_info.find_exe_path('bibtex')
    Globals.git_path_      = system_info.find_exe_path('git')    
  system_info.set_new_path()

  if Globals.compiler_path_ == "":
    return Globals.PrintError("g++ is not in the current path")
  if Globals.gfortran_path_ == "":
    return Globals.PrintError("gfortran for g++ is not installed. Please install the GCC Fortran compiler")
  if Globals.git_path_ == "":
    return Globals.PrintError("git is not in the current path. Please install a git command line client (e.g http://git-scm.com/downloads)")  
  
  if not system_info.find_gcc_version():
    return False
  return True  

"""
Get the build information from the user
"""
def start():
  build_target = ""
  build_parameters = ""
  
  if not len(sys.argv) > 1:  
    os.system( [ 'clear', 'cls' ][ os.name == 'nt' ] )
    print "*************************************************************************"
    print "*************************************************************************"
    print "Welcome to the iSAM Build System"
    print "*************************************************************************"
    print "*************************************************************************"
    print "\n"
    print "Valid build targets: " +  ", ".join(Globals.allowed_build_targets_)
    build_target = raw_input("Please enter a valid build type [debug]: ").lower()
    while build_target != "" and not build_target in Globals.allowed_build_targets_:
      print "## Error: " + build_target + " is not a valid build target" 
      build_target = raw_input("Please enter a valid build type [debug]: ").lower()
    if build_target == "":
      build_target = "debug" 
  
    # Find our build parameters to use, this will enable admb etc
    if build_target in Globals.allowed_build_types_:    
      print "Valid build parameters: " + ", ".join(Globals.allowed_build_parameters_)
      build_parameters = raw_input("Please enter build parameters [none]: ").lower()
      while build_parameters != "" and not build_parameters in Globals.allowed_build_parameters_:
        print "## Error: " + build_parameters + " is not a valid build parameter"
        build_parameters = raw_input("Please enter build parameters [none]: ").lower()
      if build_parameters == "none":
        build_parameters = ""  
  
  """
  Handle build information already passed in
  """
  if len(sys.argv) > 1 and len(str(sys.argv[1])) > 1:
      build_target = sys.argv[1]
  if len(sys.argv) > 2 and len(str(sys.argv[2])) > 1:
      build_parameters = sys.argv[2] 

  if build_target == "" or not build_target.lower() in Globals.allowed_build_targets_:
    return Globals.PrintError(build_target + " is not a valid build target")
    
  build_target = build_target.lower()    
  if build_target == "help":
    print_usage()
    return True
  if build_target == "check":
    return True 

  if build_parameters != "": 
    build_parameters = build_parameters.lower()
  
  Globals.build_target_ = build_target
  Globals.build_parameters_ = build_parameters
  
  print " -- Build target: " + Globals.build_target_
  print " -- Build parameters: " + Globals.build_parameters_
  print ""
  
  if build_target == "all":
    print "*************************************************************************"
    print "*************************************************************************"
    print "--> Starting complete isam build"
    print "--> Step 1: Third Party Libraries"
    third_party_builder = ThirdPartyLibraries()
    if not third_party_builder.start():
      return False
    
    print "--> Step 2: Debug Build"
    Globals.build_target_ = "debug"
    main_code_builder = MainCode()
    if not main_code_builder.start():
      return False
    
    print "--> Step 3: Release Build"
    Globals.build_target_ = "release"
    if not main_code_builder.start():
      return False
    
    print "--> Step 4: Test"
    Globals.build_target_ = "test"
    if not main_code_builder.start():
      return False
    
  if build_target in Globals.allowed_build_types_:      
    if not build_parameters in Globals.allowed_build_parameters_:
      return Globals.PrintError("Build parameter " + build_parameters + " is not valid")
    
    #os.system( [ 'clear', 'cls' ][ os.name == 'nt' ] )
    print "*************************************************************************"
    print "*************************************************************************"
    print "--> Starting " + Globals.build_target_ + " Build"
    code_builder = MainCode()
    if not code_builder.start():
      return False
  elif build_target == "archive":
    print "*************************************************************************"
    print "*************************************************************************"
    print "--> Starting " + Globals.build_target_ + " Build"
    archive_builder = Archiver()
    if not archive_builder.start():
      return False
  elif build_target == "thirdparty":    
    print "*************************************************************************"
    print "*************************************************************************"
    print "--> Starting " + Globals.build_target_ + " Build"
    code_builder = ThirdPartyLibraries()
    if not code_builder.start():
      return False
  elif build_target == "documentation":
    print "*************************************************************************"
    print "*************************************************************************"
    print "--> Starting " + Globals.build_target_ + " Build"
    documentation_builder = Documentation()
    documentation_builder.start()
  elif build_target == "modelrunner":
    print "*************************************************************************"
    print "*************************************************************************"
    print "--> Starting " + Globals.build_target_ + " Build"
    model_runner = ModelRunner()
    model_runner.start()
  elif build_target == "clean":
    print "*************************************************************************"
    print "*************************************************************************"
    print "--> Cleaning all iSAM built files"
    cleaner = Cleaner()
    if not cleaner.clean():
      return False
  elif build_target == "cleanall":
    print "*************************************************************************"
    print "*************************************************************************"
    print "--> Cleaning all iSAM built files, including third party headers and libs"
    cleaner = Cleaner()
    if not cleaner.clean_all():
      return False    
  
  return True
  
"""
This is the entry point in to our build system
"""
system_info = SystemInfo()
if not start_build_system():
  system_info.reset_original_path()
  exit(1)
  
exit_code = 0
if not start():
  exit_code = 1
  
system_info.reset_original_path()
print "--> Finished"
exit(exit_code)  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
 

  
