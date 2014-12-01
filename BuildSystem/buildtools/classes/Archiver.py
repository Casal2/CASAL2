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

    binary_name = 'iSAM'
    extension = ''         
    if Globals.operating_system_ == "win32":
      extension = '.exe'
      binary_name += '.exe'

    print "--> Checking for release version of iSAM binary"
    binary_path = self.output_directory_ = os.path.normpath(os.getcwd()) + "/bin/" + Globals.operating_system_ + "/release/"
    if not os.path.exists(binary_path + binary_name):
      print '-- No iSAM binary was found'
      print '-- Re-Entering the build system to build a release binary'
      print '-- All output is being diverted to release_build.log'
      if os.system(self.do_build_ + ' release > release_build.log 2>&1') != EX_OK:
        return Globals.PrintError('Failed to build release binary. Please check release_build.log for the error')
    os.system('rm -rf release_build.log')

    print '--> Checking for release version of ADOLC iSAM'
    binary_path = self.output_directory_ = os.path.normpath(os.getcwd()) + "/bin/" + Globals.operating_system_ + "/release_adolc/"
    if not os.path.exists(binary_path + binary_name):
      print '-- No iSAM binary was found'
      print '-- Re-Entering the build system to build an adolc release binary'
      print '-- All output is being diverted to release_adolc_build.log'
      if os.system(self.do_build_ + ' release adolc > release_adolc_build.log 2>&1') != EX_OK:
        return Globals.PrintError('Failed to build release binary. Please check release_adolc_build.log for the error')
    os.system('rm -rf release_adolc_build.log')    

    print '--> Checking for release version of BetaDiff iSAM'
    binary_path = self.output_directory_ = os.path.normpath(os.getcwd()) + "/bin/" + Globals.operating_system_ + "/release_betadiff/"
    if not os.path.exists(binary_path + binary_name):
      print '-- No iSAM binary was found'
      print '-- Re-Entering the build system to build a betadiff release binary'
      print '-- All output is being diverted to release_betadiff_build.log'
      if os.system(self.do_build_ + ' release betadiff > release_betadiff_build.log 2>&1') != EX_OK:
        return Globals.PrintError('Failed to build release binary. Please check release_betadiff_build.log for the error')
    os.system('rm -rf release_betadiff_build.log')

    print '--> Checking for release version of CppAD iSAM'
    binary_path = self.output_directory_ = os.path.normpath(os.getcwd()) + "/bin/" + Globals.operating_system_ + "/release_cppad/"
    if not os.path.exists(binary_path + binary_name):
      print '-- No iSAM binary was found'
      print '-- Re-Entering the build system to build a cppad release binary'
      print '-- All output is being diverted to release_cppad_build.log'
      if os.system(self.do_build_ + ' release cppad > release_cppad_build.log 2>&1') != EX_OK:
        return Globals.PrintError('Failed to build release binary. Please check release_cppad_build.log for the error')
    os.system('rm -rf release_cppad_build.log')   

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
    binary_path = self.output_directory_ = os.path.normpath(os.getcwd()) + "/bin/" + Globals.operating_system_
    
    os.system('rm -rf iSAM')
    os.system('rm -rf isam.tar')
    os.system('mkdir iSAM')
    os.system('cp ' + binary_path + '/release/' + binary_name + ' iSAM/' + binary_name)
    os.system('cp ' + binary_path + '/release_adolc/' + binary_name + ' iSAM/iSAM_adolc' + extension)
    os.system('cp ' + binary_path + '/release_betadiff/' + binary_name + ' iSAM/iSAM_betadiff' + extension)
    os.system('cp ' + binary_path + '/release_cppad/' + binary_name + ' iSAM/iSAM_cppad' + extension)
    os.system('cp ../Documentation/Manual/iSAM.pdf iSAM/iSAM.pdf')
    os.system('tar cvf isam.tar iSAM/')   
    os.system('gzip isam.tar')
    os.system('mv isam.tar.gz ' + output_directory + 'isam.tar.gz')
    os.system('rm -rf iSAM')                
    return True #Success!
    
