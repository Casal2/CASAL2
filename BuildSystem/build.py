import os
import sys
import os.path
import subprocess
import fileinput
import distutils

sys.path.insert(0, "buildtools/classes")

# These are CASAL2 specific python objects
from System import *
from Globals import *
from Builder import *
from Documentation import *
from Archiver import *
from rlibrary import *
from ModelRunner import *
from Installer import *
from DebBuilder import *

"""
Print the usage for this build system
"""
def print_usage():
  os.system( [ 'clear', 'cls' ][ os.name == 'nt' ] )
  print('###########################################################')
  print('# Casal2 Build System Usage                               #')
  print('###########################################################')
  print('Usage:')
  print('doBuild <build_target> <build_parameter>')
  print('')
  print('Valid Build Types:')
  print('  debug - Build standalone debug executable')
  print('  release - Build standalone release executable')
  print('  test - Build standalone unit tests executable')
  print('  documentation - Build the user manual')
  print('  thirdparty - Build all required third party libraries')
  print('  thirdpartylean - Build minimal third party libraries')
  print('  clean - Remove any previous debug/release build information')
  print('  cleanall - Remove all previous build information')
  print('  archive - Build a zipped archive of the application.')
  print('            Application is built using shared libraries')
  print('            so a single casal2 exectable is created.')
  print('  check - Do a check of the build system')
  print('  rlibrary - Build the R library')
  print('  modelrunner - Run the test suite of models')
  print('  installer - Build an installer package')
  print('  deb - Create linux .deb installer')
  print('  library - Build shared library for use by front end application')
  print('  frontend - Build single Casal2 executable with all minimisers ')
  print('             and unit tests')
  print('')
  print('Valid Build Parameters: (thirdparty only)')
  print('  <libary name> - Target third party library to build or rebuild')
  print('')
  print('Valid Build parameters: (debug/release only)')
  print('  adolc - Use ADOL-C automatic differentiation in compiled executable')
  print('  betadiff - Use BetaDiff automatic differentiation (from CASAL)')
  print('  cppad - Use CppAD auto-differentiation')
  print('')
  print('Valid Build parameters: (library only)')
  print('  adolc - Build ADOL-C automatic differentiation library')
  print('  betadiff - Build BetaDiff automatic differentiation library (from CASAL)')
  print('  cppad - Build CppAD automatic differentiation library')
  print('  test - Build Unit Tests library')
  print('  release - Build release library')
  print('')
  return True


"""
Start our build system. This method essentially just
gets all of the system information we need and will
test that executables are in the path that we need
"""
def start_build_system():
  if Globals.operating_system_ == "windows":
    Globals.cmd_path_      = system_info.find_exe_path('cmd.exe')
    Globals.compiler_path_ = system_info.find_exe_path('g++.exe')
    Globals.gfortran_path_ = system_info.find_exe_path('gfortran.exe')
    Globals.latex_path_    = system_info.find_exe_path('bibtex.exe')
    Globals.git_path_      = system_info.find_exe_path('git.exe')
    Globals.python_path_   = system_info.find_exe_path('python.exe')
  else:
    Globals.compiler_path_ = system_info.find_exe_path('g++')
    Globals.gfortran_path_ = system_info.find_exe_path('gfortran')
    Globals.latex_path_    = system_info.find_exe_path('bibtex')
    Globals.git_path_      = system_info.find_exe_path('git')
    Globals.python_path_   = system_info.find_exe_path('python3')
    if system_info.find_exe_path('unzip') == '':
      return Globals.PrintError('unzip is not in the current path. Please ensure it has been installed')
    if system_info.find_exe_path('cmake') == '':
      return Globals.PrintError('cmake is not in the current path. Please ensure it has been intalled')

  system_info.set_new_path()

  if Globals.compiler_path_ == "":
    return Globals.PrintError("g++ is not in the current path")
  if Globals.gfortran_path_ == "":
    return Globals.PrintError("gfortran for g++ is not installed. Please install the gcc Fortran compiler")
  if Globals.git_path_ == "":
    return Globals.PrintError("git is not in the current path. Please install a git command line client (e.g., https://git-scm.com/downloads)")
  if Globals.python_path_ == "":
    return Globals.PrintError("Python 3 is not in the current path. Please install Python 3 (e.g., https://www.python.org/downloads)")
  if Globals.operating_system_ == 'windows' and os.path.exists(Globals.git_path_ + '\\sh.exe'):
    return Globals.PrintError("git version has sh.exe in the same location. This will conflict with cmake. Please upgrade to a 64-bit version of git")
  if not system_info.find_gcc_version():
    return False

  # Check the compiler version to see if it's compatible
  pieces = Globals.compiler_version_.split('.')
  gcc_version = str(pieces[0]) + str(pieces[1])
  if gcc_version < '48':
    return Globals.PrintError("g++ version " + Globals.compiler_version_ + " is not supported due to its age")

  # check the Python version
  if sys.version_info.major == 3 and sys.version_info.minor < 6:
    return Globals.PrintError("Python 3.6 or newer is required. Please install Python 3.")

  return True

"""
Get the build information from the user
"""
def start():
  print('-- Checking for datetime Python module')
  if 'datetime' not in sys.modules:
    return Globals.PrintError("Python requires the module datetime for the build system to work")
  print('-- Checking for re Python module')
  if 're' not in sys.modules:
    return Globals.PrintError("Python requires the module re for the build system to work")
  print('-- Checking for distutils Python module')
  if 'distutils' not in sys.modules:
    return Globals.PrintError("Python requires the module distutils for the build system to work")

  build_target = ""
  build_parameters = ""

  """
  Handle build information already passed in
  """
  if len(sys.argv) > 1 and len(str(sys.argv[1])) > 1:
      build_target = sys.argv[1]
  if len(sys.argv) > 2 and len(str(sys.argv[2])) > 1:
      build_parameters = sys.argv[2]

  if build_target == "":
    return Globals.PrintError('Please provide a valid build target. Use doBuild to see the list');
  if not build_target.lower() in Globals.allowed_build_targets_:
    return Globals.PrintError(build_target + " is not a valid build target")

  build_target = build_target.lower()
  if build_target == "help":
    print_usage()
    return True
  if build_target == "check":
    print("--> All checks completed successfully")
    return True

  if build_parameters != "":
    build_parameters = build_parameters.lower()

  Globals.build_target_ = build_target
  Globals.build_parameters_ = build_parameters

  print(" -- Build target: " + Globals.build_target_)
  print(" -- Build parameters: " + Globals.build_parameters_)
  print("")
  print("*************************************************************************")
  print("*************************************************************************")
  if build_target in Globals.allowed_build_types_:
    if not build_parameters in Globals.allowed_build_parameters_:
      return Globals.PrintError("Build parameter " + build_parameters + " is not valid")

    print("--> Starting " + Globals.build_target_ + " Build")
    code_builder = MainCode()
    if not code_builder.start(False):
      return False
  if build_target == "library":
    if not build_parameters in Globals.allowed_library_parameters_:
      return Globals.PrintError("Library build parameter" + build_parameters + " is not valid")
    print("--> Starting " + Globals.build_target_ + " Build")
    code_builder = MainCode()
    if not code_builder.start(True):
      return False
  elif build_target == "frontend":
    print("--> Starting " + Globals.build_target_ + " Build")
    code_builder = FrontEnd()
    if not code_builder.start():
      return False
  elif build_target == "archive":
    print("--> Starting " + Globals.build_target_ + " Build")
    archive_builder = Archiver()
    if not archive_builder.start(build_parameters):
      return False
  elif build_target == "thirdparty" or build_target == "thirdpartylean":
    print("--> Starting " + Globals.build_target_ + " Build")
    code_builder = ThirdPartyLibraries()
    if not code_builder.start():
      return False
  elif build_target == "documentation":
    print("--> Starting " + Globals.build_target_ + " Build")
    documentation_builder = Documentation()
    if not documentation_builder.start():
      return False
  elif build_target == "modelrunner":
    print("--> Starting " + Globals.build_target_ + " Build")
    model_runner = ModelRunner()
    if not model_runner.start():
      return False
  elif build_target == "clean":
    print("--> Cleaning all Casal2 built files")
    cleaner = Cleaner()
    if not cleaner.clean():
      return False
  elif build_target == "cleanall":
    print("--> Cleaning all Casal2 built files, including third-party headers and libraries")
    cleaner = Cleaner()
    if not cleaner.clean_all():
      return False
  elif build_target == "rlibrary":
    print("--> Starting " + Globals.build_target_ + " Build"   )
    r_path = ''
    if Globals.operating_system_ == 'windows':
      print("find Windows R")
      r_path = system_info.find_exe_path('R.exe')
    else:
      r_path = system_info.find_exe_path('R')
    print("R path = " + r_path  )
    if r_path == '':
      return Globals.PrintError("R is not in the current path. Please install R or add R to the path.")
    rlibrary = Rlibrary()
    if not rlibrary.start():
      return False
  elif build_target == "installer":
    print("--> Building Casal2 Windows Installer")
    if Globals.operating_system_ == 'linux':
      return Globals.PrintError('Building Windows installer under Linux is not supported')
    installer = Installer()
    installer.start(build_parameters)
  elif build_target == "deb":
    print("--> Building Casal2 Linux .deb Installer"    )
    if Globals.operating_system_ == 'windows':
      return Globals.PrintError('Building Linux .deb package under Windows is not supported')
    deb_builder = DebBuilder()
    if not deb_builder.start(build_parameters):
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
print("--> Finished ")
exit(exit_code)
