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
    
  def set_new_path(self):
    print "-- Overriding the system path with new values"
    os.environ['PATH'] = Globals.path_ + self.original_path_
    
  def reset_original_path(self):
    os.environ['PATH'] = self.original_path_
    
  """
  This method will get the path of the compiler for us and pre-prepend it to the
  operating systems current path so we can use it to build the project
  """
  def find_compiler_path(self):
    print "-- Searching for g++ in the path"
    # Find our Compiler and put it's location in the path
    compiler = 'g++'
    if Globals.operating_system_ == "win32":
      compiler += ".exe"  
        
    if os.system('which ' + compiler + ' > temp_compiler.txt') != EX_OK:
      print '## ERROR: Failed to execute \'which\' command to find the currently installed compiler'
      return False
        
    fi = fileinput.FileInput('temp_compiler.txt')
    
    compiler_path = ''    
    for line in fi:
        compiler_path = line
            
    compiler_path = compiler_path.replace(compiler + '\n', '').rstrip()
    print "-- g++ found @ " + compiler_path
    Globals.compiler_path_ = compiler_path
    Globals.path_ = compiler_path + ";" + Globals.path_ 
    self.set_new_path()
    
    if os.system('rm -rf temp_compiler.txt') != EX_OK:
      print '## ERROR: Failed to remove the compiler.txt after reading the compiler path'
      return False
    
    return True
  
  """
  This method will find the GCC Version
  """
  def find_gcc_version(self):
    p = subprocess.Popen(["g++", "--version"], stdout=subprocess.PIPE)
    out, err = p.communicate()
    lines = out.split('\n')
    pieces = lines[0].split()
    if len(pieces) != 3:
      print '## ERROR: STD out did not return correct GCC Version format (' + str(len(pieces)) + ': ' + lines[0]
      return False


    Globals.compiler_version_ = pieces[2].lstrip().rstrip()
    print '--> Compiler Version: ' + Globals.compiler_version_
    
    return True
  
  """
  This method will find the path of cmd.exe so we can use it to call
  methods that we require
  """
  def find_cmd_path(self):
    print "--- Searching for cmd in the path"
    if os.system('which cmd.exe > temp_cmd.txt') != EX_OK:
      print '## ERROR: Failed to execute \'which\' command to find the currently installed cmd.exe path'
      return False
        
    fi = fileinput.FileInput('temp_cmd.txt')
    cmd_path = ''
    for line in fi:
        cmd_path = line

    cmd_path = cmd_path.replace('cmd.exe\n', '').rstrip()
    print "--- cmd found @ " + Globals.cmd_path_
    Globals.cmd_path_ = cmd_path
    Globals.path_ += cmd_path + ";"   
    self.set_new_path()      

    if os.system('rm -rf temp_cmd.txt') != EX_OK:
      print '## ERROR: Failed to remove the cmd.txt after reading the cmd.exe path'
      return False
    
    return True
