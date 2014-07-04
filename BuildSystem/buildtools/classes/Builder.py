import os
import sys
import subprocess
import os.path
import shutil

import Globals

EX_OK = getattr(os, "EX_OK", 0)

"""
This class is responsible for building the main code base of the project
"""
class MainCode:
  cmake_compiler_ = ""
  output_directory_ = "";
  
  def start(self):
    print "--> Starting build of the main code base"
    print "--> Build configuration " + Globals.build_target_ + " : " + Globals.build_parameters_
    print "--> Operating System: " + Globals.operating_system_
    
    self.cmake_compiler_ = [ 'Unix', 'MinGW' ][ Globals.operating_system_ == "win32" ]
    print "--> CMake Compiler: " + self.cmake_compiler_
    
    # Check to see if the third party libraries have been built
    third_party_dir = "bin/" + Globals.operating_system_ + "/thirdparty"
    if not os.path.exists(third_party_dir):
      return Globals.PrintError("Third party libraries have not been built. Please build these first with thirdparty arguement")
          
    self.output_directory_ = "bin/" + Globals.operating_system_ + "/" + Globals.build_target_ 
    if Globals.build_parameters_ != "":
      self.output_directory_ += "_" + Globals.build_parameters_
      
    if not os.path.exists(self.output_directory_):
      os.makedirs(self.output_directory_);
    print "--> Target output directory: " + self.output_directory_
    
    os.chdir(self.output_directory_);
    
    print '--> Preparing CMake command'
    build_string = 'cmake -G "' + self.cmake_compiler_ + ' Makefiles" -D' + Globals.build_target_.upper() + '=1'
    if Globals.build_parameters_ != "":
      build_string += ' -D' + Globals.build_parameters_.upper() + '=1'
    build_string += ' ../../..'
    
    print "--> CMake command: " + build_string
    if os.system(build_string) != EX_OK:
      return Globals.PrintError("Failed to execute cmake successfully to rebuild the make files")
    
    print "--> Build main code base"
    if Globals.operating_system_ == "win32":
      if os.system("mingw32-make") != EX_OK:
        return Globals.PrintError("Failed to build code base. Please see above for build error")
    else:
      if os.system("make") != EX_OK:
        return Globals.PrintError("Failed to build code base. Please see above for build error")
    
    return True    

"""
This class is responsible for build the third party libraries part of the code base
"""
class ThirdPartyLibraries:
  input_directory = ""
  output_directory_ = ""
  include_directory = ""
  lib_directory     = ""
  lib_debug_directory = ""
  lib_release_directory = ""
  
  def start(self):
    print "--> Starting build of the third party libraries"
    print "-- Operating System: " + Globals.operating_system_
  
    print "-- Checking if output folder structure exists"  
    self.output_directory_ = os.path.normpath(os.getcwd()) + "/bin/" + Globals.operating_system_ + "/thirdparty"
    if not os.path.exists(self.output_directory_):
      print "-- Creating output directory: " + self.output_directory_
      os.makedirs(self.output_directory_)
      
    self.include_directory = self.output_directory_ + "/include"
    if not os.path.exists(self.include_directory):
      print "-- Creating include directory: " + self.include_directory
      os.makedirs(self.include_directory)
      
    self.lib_directory = self.output_directory_ + "/lib"
    if not os.path.exists(self.lib_directory):
      print "-- Creating lib directory: " + self.lib_directory
      os.makedirs(self.lib_directory)
    
    self.lib_debug_directory = self.lib_directory + "/debug"
    if not os.path.exists(self.lib_debug_directory):
      print "-- Creating lib debug directory: " + self.lib_debug_directory
      os.makedirs(self.lib_debug_directory)
      
    self.lib_release_directory = self.lib_directory + "/release"
    if not os.path.exists(self.lib_release_directory):
      print "-- Creating lib release directory: " + self.lib_release_directory
      os.makedirs(self.lib_release_directory)
      
    self.input_directory = "../ThirdParty/"
    third_party_list = os.listdir(self.input_directory)
    
    Globals.target_success_path_      = self.output_directory_ + '/'
    Globals.target_include_path_      = self.output_directory_ + '/include/'
    Globals.target_debug_lib_path_    = self.output_directory_ + '/lib/debug/'
    Globals.target_release_lib_path_  = self.output_directory_ + '/lib/release/'
    
    cwd = os.path.normpath(os.getcwd())    
    build_module_name = "build"    
    
    found_build = False
    
    for folder in third_party_list:
      # Do some simple checks to see if we want to process this folder
      if folder.startswith("."):
        continue
      if Globals.build_parameters_ != "" and Globals.build_parameters_ != folder.lower():
        continue                 
      if folder.startswith("test-") and Globals.build_parameters_ == "":
        print '--> Skipping library ' + folder + ' (test library)'
        continue      
      
      # load the version number so we can handle upgrades
      library_version = str(-1.0)
      success_file = self.output_directory_ + "/" + folder + ".success"
      if os.path.exists(success_file):
        f = open(success_file, "r")
        library_version = f.readline().lstrip().rstrip().replace('\n', '')    
        f.close()    
      
      found_build = True      
      target_folder = "../ThirdParty/" + folder
      success = False
                  
      os.chdir(target_folder)
      sys.path.append(os.path.normpath(os.getcwd()))

      """
      # Handle loading the windows file and building this on windows
      """
      if Globals.operating_system_ == "win32":
        if not os.path.exists('windows.py'):
          return Globals.PrintError('Third party library ' + folder + ' does not have a windows.py file.\nThis file is required to build this library on Windows')
        import windows as third_party_builder
        builder = third_party_builder.Builder()        
        if os.path.exists(success_file) and Globals.build_parameters_ == "":
          if str(library_version) == str(builder.version_) and str(library_version) != str(-1.0):
            print '--> Skipping library ' + folder + ' (version already installed)'
            success = True
        else:
          print ""
          print "--> Building Library: " + folder
          success = builder.start()
          if success:
            os.system('echo ' + str(builder.version_) + ' > ' + Globals.target_success_path_ + folder + '.success')
            print ""
        del sys.modules["windows"]
        
      else:
        if not os.path.exists('linux.py'):
          return Globals.PrintError('Third party library ' + folder + ' does not have a linux.py file.\nThis file is required to build this library on Linux')
        import linux as third_party_builder
        builder = third_party_builder.Builder()        
        if os.path.exists(success_file) and Globals.build_parameters_ == "":
          if str(library_version) == str(builder.version_) and str(library_version) != str(-1.0):
            print '--> Skipping library ' + folder + ' (version already installed)'
            success = True
        else:
          print ""
          print "--> Building Library: " + folder
          success = builder.start()
          if success:
            os.system('echo ' + str(builder.version_) + ' > ' + Globals.target_success_path_ + folder + '.success')
            print ""
        del sys.modules["linux"]
            
      sys.path.pop()
      os.chdir(cwd)                
      if not success:
        return False
    
    if not found_build and Globals.build_parameters_ != "":
      return Globals.PrintError("Third party library " + Globals.build_parameters_ + " does not exist")
    
    print ""
    print "--> All third party libraries have been built successfully"
    return True

"""
This class is responsible for cleaning the build folders
"""   
class Cleaner:    
  def clean(self):
    print '--> Starting clean of iSAM build files only (not cleaning third party)'      
    for build_type in Globals.allowed_build_types_:    
      for param in Globals.allowed_build_parameters_:
        build_directory = os.path.normpath(os.getcwd()) + "/bin/" + Globals.operating_system_ + '/' + build_type
        if param != "":
          build_directory = build_directory + "_" + param
        if (os.path.exists(build_directory)):
          print '--> Deleting folder: ' + build_directory
          shutil.rmtree(build_directory)
          
  def clean_all(self):
    print '--> Starting clean of all iSAM build files (including third party)'
    build_directory = os.path.normpath(os.getcwd()) + "/bin/" + Globals.operating_system_
    if (os.path.exists(build_directory)):
      shutil.rmtree(build_directory)
    