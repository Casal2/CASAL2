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
    print '--> Starting SystemInfo object'
    Globals.operating_system_ = sys.platform
    self.original_path_    = os.getenv('PATH')
    
    if (Globals.operating_system_.startswith("linux")):
      Globals.operating_system_ = "linux"
      
    print "-- Configuring for Operating System: " + Globals.operating_system_
    cwd = os.path.normpath(os.getcwd())
    if (Globals.operating_system_ == "win32"):
      Globals.path_ += cwd + '\\buildtools\\windows\\unixutils;'
      Globals.path_ += cwd + '\\buildtools\\windows\\cmake\\bin;'   
      Globals.path_ += cwd + '\\buildtools\\windows\\Python27\\;'
      Globals.path_ += cwd + '\\bin\\' + Globals.operating_system_ + '\\debug;'
    else:
      Globals.path_ += cwd + '/bin/' + Globals.operating_system_ + '/debug'
    
  def set_new_path(self):
    print "-- Overriding the system path with new values"
    if Globals.operating_system_ == "win32":
      os.environ['PATH'] = Globals.path_
      print '-- New Path: ' + Globals.path_
    else:
      os.environ['PATH'] = Globals.path_ + ":" + self.original_path_
      print '-- New Path: ' + Globals.path_ + ":" + self.original_path_    
    
  def reset_original_path(self):
    os.environ['PATH'] = self.original_path_
    
  """
  This method will search the path for a specific EXE and then ensure we put it in to our
  path
  """
  def find_exe_path(self, exe):
    print '-- Searching path for ' + exe
    if os.system('which ' + exe + ' > which.tmp 2> err.tmp') != EX_OK:
      os.system('rm -rf err.tmp')
      print '## ' + exe + ' not found in the current path'
      return ''
      
    # Read path back from file
    fi = fileinput.FileInput('which.tmp')    
    path = ''
    for line in fi:
        path = line
    path = path.replace(exe + '\n', '').rstrip()
    print '-- ' + exe + ' found @ ' + path

    if path != "":
      Globals.path_ = path + ";" + Globals.path_

    os.system('rm -rf err.tmp')      
    if os.system('rm -rf which.tmp') != EX_OK:
      Globals.PrintError('Could not delete the temporary file "which.tmp". Do you have correct file system access?')
      return
    
    return path
  
  """
  This method will find the GCC Version
  """
  def find_gcc_version(self):
    p = subprocess.Popen(["g++", "-v"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()
    lines = out.split('\n')
    err_lines = re.split('version', err)
    target_line = err_lines[len(err_lines) - 1].lstrip().rstrip()
    print '--> Full Version: ' + target_line
    pieces = target_line.split(' ')
    if len(pieces) < 2:
      return Globals.PrintError('STD out did not return correct GCC Version format ' + str(len(pieces)) + ': ' + target_line)

    Globals.compiler_version_ = pieces[0].lstrip().rstrip()
    print '--> Compiler Version: ' + Globals.compiler_version_
    
    return True
  
