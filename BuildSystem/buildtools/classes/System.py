import os
import sys
import os.path
import subprocess
import fileinput
import re
import Globals # Global Variables for build system

EX_OK = getattr(os, "EX_OK", 0)

class SystemInfo:
  # Variables
  original_path_ = "" 
    
  # Constructor
  def __init__(self):
    print('--> Starting SystemInfo object')
    Globals.operating_system_ = sys.platform
    self.original_path_    = os.getenv('PATH')
    
    if (Globals.operating_system_.startswith("linux")):
      Globals.operating_system_ = "linux"
    elif (Globals.operating_system_.startswith("darwin")):
      Globals.operating_system_ = "darwin"
    elif (Globals.operating_system_.startswith("win")):
      Globals.operating_system_ = "windows"
      
    print( "-- Configuring for Operating System: " + Globals.operating_system_)
    cwd = os.path.normpath(os.getcwd())
    if (Globals.operating_system_ == "windows"):
      Globals.path_ += cwd + '\\buildtools\\windows\\unixutils;'
      Globals.path_ += cwd + '\\buildtools\\windows\\cmake\\bin;'   
      Globals.path_ += cwd + '\\bin\\' + Globals.operating_system_ + '\\release_betadiff;'
    else:
      Globals.path_ += cwd + '/bin/' + Globals.operating_system_ + '/release_betadiff'

    print("-- Checking if number of build threads has been specified in CASAL2_BUILD_THREADS")
    if (os.environ.get("CASAL2_BUILD_THREADS") != None):
      Globals.threads_ = int(os.environ['CASAL2_BUILD_THREADS'])
      print(f"-- Number of build threads set to {Globals.threads_}")
    else:
      print("-- Setting default number of build threads to 8")
      Globals.threads_ = 8

  """
  Populate the Global variables with the different EXE locations based on the Operating
  System
  """
  def set_required_tools_paths(self):
    print("--> Setting the paths for the required tools")
    self.determine_compiler();

    if Globals.operating_system_ == "windows":
      Globals.cmd_path_      = self.find_exe_path('cmd.exe')
      Globals.python_cmd_    = "python"
      Globals.gfortran_path_ = self.find_exe_path('gfortran.exe')
      Globals.latex_path_    = self.find_exe_path('bibtex.exe')
      Globals.git_path_      = self.find_exe_path('git.exe')    
    else:
      Globals.python_cmd_    = "python3"
      Globals.gfortran_path_ = self.find_exe_path('gfortran')
      Globals.latex_path_    = self.find_exe_path('bibtex')
      Globals.git_path_      = self.find_exe_path('git')    
      if self.find_exe_path('unzip') == '':
        return Globals.PrintError('unzip is not in the current path. Please ensure it has been installed')
      if self.find_exe_path('cmake') == '':
        return Globals.PrintError('cmake is not in the current path. Please ensure it has been intalled')    

    self.set_new_path()

    if Globals.git_path_ == "":
      return Globals.PrintError("git is not in the current path. Please install a git command line client (e.g http://git-scm.com/downloads)")  
    if Globals.operating_system_ == 'windows' and os.path.exists(Globals.git_path_ + '\\sh.exe'):
      return Globals.PrintError("git version has sh.exe in the same location. This will conflict with cmake. Please upgrade to a 64bit version of Git")    
    if not self.find_compiler_version():
      return False

    return True  

  """
  Override the environment path including the packages we've included with this
  source base. For Windows we've included unix utilities and cmake
  """
  def set_new_path(self):
    print("-- Overriding the system path with new values")
    if Globals.operating_system_ == "windows":
      os.environ['PATH'] = Globals.path_ + ";" + self.original_path_
    else:
      os.environ['PATH'] = Globals.path_ + ":" + self.original_path_
    print('-- New Path: ' + os.environ['PATH'])
    
  """
  Reset the path to what was there before we started the build system.
  """
  def reset_original_path(self):
    os.environ['PATH'] = self.original_path_
    
  """
  This method will search the path for a specific EXE and then ensure we put it in to our
  path
  """
  def find_exe_path(self, exe):
    print('-- Searching path for ' + exe)
    if os.system('which ' + exe + ' > which.tmp 2> err.tmp') != EX_OK:
      os.system('rm -rf err.tmp')
      print('## ' + exe + ' not found in the current path')
      return ''
      
    # Read path back from file
    fi = fileinput.FileInput('which.tmp')    
    path = ''
    for line in fi:
        path = line
    path = path.replace(exe + '\n', '').rstrip()

    if Globals.operating_system_ == "windows" and path.startswith("/c/"):
      path = path.replace("/c/", "c:/")
      path = path.replace("/", "\\")
      
    print('-- ' + exe + ' found @ ' + path)

    if path != "" and Globals.operating_system_ == "windows":
      Globals.path_ = path + ";" + Globals.path_
    elif path != "":
      Globals.path_ = path + ":" + Globals.path_

    os.system('rm -rf err.tmp')      
    if os.system('rm -rf which.tmp') != EX_OK:
      Globals.PrintError('Could not delete the temporary file "which.tmp". Do you have correct file system access?')
      return
    
    return path
	
  """
  This method will determine the compiler to use
  """
  def determine_compiler(self):
    ext = ""
    if Globals.operating_system_ == "windows":
      ext = ".exe"
    # Check for GCC
    target = "g++" + ext
    p = subprocess.Popen(["which", target], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()
    if p.wait() == 0:
      Globals.compiler_ = "gcc"
      Globals.compiler_path_ = out.decode("utf-8").rstrip() 
      if Globals.operating_system_ == "windows":
        Globals.make_command_ = "mingw32-make -j " + str(Globals.threads_)
      else:
        Globals.make_command_ = "make -j " + str(Globals.threads_)

      if Globals.compiler_path_.startswith("/c/"):
        Globals.compiler_path_ = Globals.compiler_path_.replace("/c/", "c:/")
        Globals.compiler_path_ = Globals.compiler_path_.replace("/", "\\")

      print("-- G++ Path: " + Globals.compiler_path_)
      return

    # Check for Visual Studio
    target = "cl" + ext
    p = subprocess.Popen(["which", target], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()
    if p.wait() == 0:
      Globals.compiler_ = "msvc"
      Globals.compiler_path_ = out.decode("utf-8").rstrip()
      Globals.make_command_ = "devenv Casal2.sln /Build Release" 
      print("-- Visual Studio Path: " + out.decode("utf-8").rstrip())
      return   

    # Check for Clang
    target = "clang++" + ext
    p = subprocess.Popen(["which", target], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()
    if p.wait() == 0:
      Globals.compiler_ = "clang"
      Globals.compiler_path_ = out.decode("utf-8").rstrip() 
      if Globals.operating_system_ == "windows":
        Globals.make_command_ = "mingw32-make -j " + str(Globals.threads_)
      else:
        Globals.make_command_ = "make -j " + str(Globals.threads_)
      print("-- Clang Path: " + out.decode("utf-8").rstrip())
      return
    
    Globals.PrintError("Could not find a suitable compiler. Checked for clang++, g++ and MS Visual Studio")   
    exit(-1)

  """
  Find out Compiler version
  """
  def find_compiler_version(self):
    if Globals.operating_system_ == "windows":
      Globals.compiler_path_ = Globals.compiler_path_.replace("/", "\\")

    self.get_cmake_compiler_string()

    if Globals.compiler_ == "clang":
      return self.find_clang_version()
    elif Globals.compiler_ == "gcc":
      return self.find_gcc_version()
    elif Globals.compiler_ == "msvc":
      return self.find_msvc_version()
    
    return False


  """
  Find the version of the Clang++ compiler we're using
  """
  def find_clang_version(self):
    print("--> Searching for clang compiler verstion in '" + Globals.compiler_path_ + "'")
    p = subprocess.Popen([Globals.compiler_path_, "-v"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()
    if (p.wait() != 0):
      return Globals.PrintError("clang++ did not return expected output format to determine the version")

    version = str(err).split(' ')[2]
    Globals.compiler_version_ = version.lstrip().rstrip()
    print("--> clang++ version: " + version)

    pieces = Globals.compiler_version_.split('.')
    if int(pieces[0]) < 9:
      return Globals.PrintError("clang++ version " + Globals.compiler_version_ + " is not supported due to it's age")

    return True

  """
  This method will find the GCC Version
  """
  def find_gcc_version(self):
    print("--> Searching for g++ compiler verstion in '" + Globals.compiler_path_ + "'")
    p = subprocess.Popen([Globals.compiler_path_, "-v"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()
    err_lines = re.split('version', str(err))
    target_line = err_lines[len(err_lines) - 1].lstrip().rstrip()
    print('--> Full Version: ' + target_line)
    pieces = target_line.split(' ')
    if len(pieces) < 2:
      return Globals.PrintError('STD out did not return correct GCC Version format ' + str(len(pieces)) + ': ' + target_line)

    Globals.compiler_version_ = pieces[0].lstrip().rstrip()
    print('--> gcc version: ' + Globals.compiler_version_)

    pieces = Globals.compiler_version_.split('.')
    if int(pieces[0]) < 7:
      return Globals.PrintError("g++ version " + Globals.compiler_version_ + " is not supported due to it's age")

    return True

  """
  Find the version of the Microsoft Visual Studio Compiler
  """
  def find_msvc_version(self):
    print("--> Searching for msvc compiler verstion in '" + Globals.compiler_path_ + "'")
    p = subprocess.Popen([Globals.compiler_path_, "/?"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()
    if (p.wait() != 0):
      return Globals.PrintError("msvc did not return expected output format to determine the version")

    version = str(err).split(' ')[6]
    Globals.compiler_version_ = version.lstrip().rstrip()
    print("--> msvc version: " + version)

    pieces = Globals.compiler_version_.split('.')
    if int(pieces[0]) < 19:
      return Globals.PrintError("msvc version " + Globals.compiler_version_ + " is not supported due to it's age")

    return True   

    """
    Get the compiler string to use for CMake
    """
  def get_cmake_compiler_string(self):
    if Globals.compiler_ == "clang":
      Globals.cmake_compiler_ = '-DOUTPUT_PATH:STRING=windows_clang'
    elif Globals.compiler_ == "gcc" and Globals.operating_system_ == "windows":
      Globals.cmake_compiler_ = '-G "MinGW Makefiles" -DOUTPUT_PATH:STRING=windows_gcc'
    elif Globals.compiler_ == "gcc" and Globals.operating_system_ == "linux":
      Globals.cmake_compiler_ = '-G "Unix Makefiles" -DOUTPUT_PATH:STRING=linux_gcc'
    elif Globals.compiler_ == "msvc":
      Globals.cmake_compiler_ = '-DOUTPUT_PATH:STRING=windows_msvc'
    else:
      Globals.cmake_compiler_ = "get_cmake_compiler_string() error"
    
    return True
  
