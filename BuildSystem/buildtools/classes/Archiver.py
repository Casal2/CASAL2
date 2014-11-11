import os
import sys
import subprocess
import os.path
import shutil
import fileinput
import re
from datetime import datetime, date
from dateutil import tz

import Globals
from Builder import *

EX_OK = getattr(os, "EX_OK", 0)

"""
This class is responsible for building the main code base of the project
"""
class Archiver:
  cmake_compiler_ = ""
  output_directory_ = "";
  do_build_ = "doBuild"
  def start(self):
    if Globals.operating_system_ == "win32":
      self.do_build_ += '.bat'
    else:
      self.do_build_ = './' + self.do_build_ + '.sh'
    
    print "--> Archiving any previously built binaries"
    print "--> Checking if Third Party libraries have been built"
    third_party_dir = "bin/" + Globals.operating_system_ + "/thirdparty"
    if not os.path.exists(third_party_dir):
      print '-- Re-Entering build system to build the third party libraries'
      print '-- All output is being diverted to third_party.log'
      print '-- Expected build time 10-60 minutes'
      if os.system(self.do_build_ + ' thirdparty > third_party.log 2>&1') != EX_OK:
        return Globals.PrintError('Failed to build the third party libraries. Please check third_party.log for the error')      
    os.system('rm -rf third_party.log')

    print "--> Checking for release version of iSAM binary"
    binary_path = self.output_directory_ = os.path.normpath(os.getcwd()) + "/bin/" + Globals.operating_system_ + "/release/isam"
    binary_name = 'iSAM'
    if Globals.operating_system_ == "win32":
      binary_path += ".exe"
      binary_name += '.exe'
    if not os.path.exists(binary_path):
      print '-- No iSAM binary was found'
      print '-- Re-Entering the build system to build a release binary'
      print '-- All output is being diverted to release_build.log'
      if os.system(self.do_build_ + ' release > release_build.log 2>&1') != EX_OK:
        return Globals.PrintError('Failed to build release binary. Please check release_build.log for the error')
    os.system('rm -rf release_build.log')

    print '--> Building documentation'
    print '-- Re-Entering the build system to build the documentation'
    print '-- All ourput is being diverted to documentation_build.log'
    if os.system(self.do_build_ + ' documentation > documentation_build.log 2>&1') != EX_OK:
      return Globals.PrintError('Failed to build the documentation. Please check documenation_build.log for error')
    os.system('rm -rf documentation_build.log')

    ## Now we actually do the zipping of the binary
    output_directory = "bin/" + Globals.operating_system_ + "/archive/"
    if not os.path.exists(output_directory):
      os.makedirs(output_directory);
    print "-- Target output directory: " + output_directory

    if os.path.exists(output_directory + "isam.tar.gz"):
      print "-- Removing old Archive"
      os.system("rm -rf " + output_directory + "isam.tar.gz")

    # Shitty stuff to make building archive easier
    os.system('rm -rf iSAM')
    os.system('rm -rf isam.tar')
    os.system('mkdir iSAM')
    os.system('cp ' + binary_path + ' iSAM/' + binary_name)
    os.system('cp ../Documentation/Manual/iSAM.pdf iSAM/iSAM.pdf')
    os.system('tar cvf isam.tar iSAM/')   
    os.system('gzip isam.tar')
    os.system('mv isam.tar.gz ' + output_directory + 'isam.tar.gz')
    os.system('rm -rf iSAM')                
    return True #Success!
    
