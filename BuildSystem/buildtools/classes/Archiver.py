import os
import sys
import subprocess
import os.path
import shutil
import fileinput
import re
from datetime import datetime, date

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
  def start(self, skip_building = 'false'):
    if Globals.operating_system_ == "windows":
      self.do_build_ += '.bat'
    else:
      self.do_build_ = './' + self.do_build_ + '.sh'

    library_name = ''
    binary_name = 'casal2'
    extension = ''
    if Globals.operating_system_ == "windows":
      library_name = 'libcasal2.dll'
      binary_name += '.exe'
      extension = '.dll'
    else:
      library_name = 'libcasal2.so'
      extension = '.so'

    if skip_building != 'true':
      print('--> Building third party libraries')
      print('-- Re-Entering build system to build the third party libraries')
      print('-- All output is being diverted to third_party.log')
      print('-- Expected build time 10-60 minutes')
      if os.system(self.do_build_ + ' thirdparty > third_party.log 2>&1') != EX_OK:
        return Globals.PrintError('Failed to build the third party libraries. Please check third_party.log for the error')
      os.system('rm -rf third_party.log')

      print('--> Building release version of CASAL2 library')
      print('-- Re-Entering the build system to build a release library')
      print('-- All output is being diverted to release_build.log')
      if os.system(self.do_build_ + ' library release > release_build.log 2>&1') != EX_OK:
        return Globals.PrintError('Failed to build release library. Please check release_build.log for the error')
      os.system('rm -rf release_build.log')

      print('--> Building release version of ADOLC CASAL2')
      print('-- Re-Entering the build system to build an adolc release library')
      print('-- All output is being diverted to release_adolc_build.log')
      if os.system(self.do_build_ + ' library adolc > release_adolc_build.log 2>&1') != EX_OK:
        return Globals.PrintError('Failed to build release library. Please check release_adolc_build.log for the error')
      os.system('rm -rf release_adolc_build.log')

      print('--> Building release version of BetaDiff CASAL2')
      print('-- Re-Entering the build system to build a betadiff release library')
      print('-- All output is being diverted to release_betadiff_build.log')
      if os.system(self.do_build_ + ' library betadiff > release_betadiff_build.log 2>&1') != EX_OK:
        return Globals.PrintError('Failed to build release library. Please check release_betadiff_build.log for the error')
      os.system('rm -rf release_betadiff_build.log')

      print('--> Building release version of CppAD CASAL2')
      print('-- Re-Entering the build system to build a cppad release library')
      print('-- All output is being diverted to release_cppad_build.log')
      if os.system(self.do_build_ + ' library cppad > release_cppad_build.log 2>&1') != EX_OK:
        return Globals.PrintError('Failed to build release library. Please check release_cppad_build.log for the error')
      os.system('rm -rf release_cppad_build.log')

      print('--> Building documentation')
      print('-- Re-Entering the build system to build the documentation')
      print('-- All output is being diverted to documentation_build.log')
      if os.system(self.do_build_ + ' documentation > documentation_build.log 2>&1') != EX_OK:
        return Globals.PrintError('Failed to build the documentation. Please check documenation_build.log for error')
      os.system('rm -rf documentation_build.log')

      print('--> Building test version of CASAL2')
      print('-- Re-Entering the build system to build a unit test library')
      print('-- All output is being diverted to unit_test_build.log')
      if os.system(self.do_build_ + ' library test > unit_test_build.log 2>&1') != EX_OK:
        return Globals.PrintError('Failed to build release library. Please check unit_test_build.log for the error')
      os.system('rm -rf unit_test_build.log')

    print('--> Building Front End Binary')
    print('-- Re-Entering the build system to build the front end binary')
    print('-- All output is being diverted to front_end_build.log')
    if os.system(self.do_build_ + ' frontend > front_end_build.log 2>&1') != EX_OK:
      return Globals.PrintError('Failed to build the front end binary. Please check front_end_build.log for error')
    os.system('rm -rf front_end_build.log')

    ## Now we actually do the zipping of the binary
    output_directory = "bin/" + Globals.operating_system_ + "/archive/"
    if not os.path.exists(output_directory):
      os.makedirs(output_directory);
    print("-- Target output directory: " + output_directory)

    if os.path.exists(output_directory + "casal2.tar.gz"):
      print("-- Removing old Archive")
      os.system("rm -rf " + output_directory + "casal2.tar.gz")

    # Shitty stuff to make building archive easier
    binary_path = self.output_directory_ = os.path.normpath(os.getcwd()) + "/bin/" + Globals.operating_system_

    os.system('rm -rf Casal2')
    os.system('rm -rf casal2.tar')
    os.system('rm -rf casal2.zip')
    os.system('mkdir Casal2')
    os.makedirs('Casal2/R-Libraries')
    os.system('cp ' + binary_path + '/frontend/' + binary_name + ' Casal2/' + binary_name)
    os.system('cp ' + binary_path + '/library_release/' + library_name + ' Casal2/casal2_release' + extension)
    os.system('cp ' + binary_path + '/library_adolc/' + library_name + ' Casal2/casal2_adolc' + extension)
    os.system('cp ' + binary_path + '/library_betadiff/' + library_name + ' Casal2/casal2_betadiff' + extension)
    os.system('cp ' + binary_path + '/library_cppad/' + library_name + ' Casal2/casal2_cppad' + extension)
    os.system('cp ' + binary_path + '/library_test/' + library_name + ' Casal2/casal2_test' + extension)
    os.system('cp ../Documentation/UserManual/CASAL2.pdf Casal2/"Casal2 User Manual.pdf"')
    print("-- Manual copied")
    os.system('cp ../Documentation/GettingStartedGuide/GettingStartedGuide.pdf Casal2/GettingStartedGuide.pdf')
    print("-- Starters' guide copied")
    os.system('cp ../Documentation/ContributorsManual/ContributorsGuide.pdf Casal2/ContributorsGuide.pdf')    
    print("-- Contributors' guide copied")
    os.system('cp ../Documentation/UserManual/CASAL2.syn Casal2/CASAL2.syn')    
    os.system('cp ../Documentation/UserManual/TextPad_syntax_highlighter.readme Casal2/TextPad_syntax_highlighter.readme')        
    os.system('cp -r ../Examples Casal2/Examples')
    print("-- Example code copied"    )
    os.system('cp -r ../README.txt Casal2/README.txt')
    ## deal with R libraries
    if Globals.operating_system_ == "windows":
      os.system('cp ../R-libraries/casal2_1.0.zip Casal2/R-Libraries/casal2.zip')
    else: 
      os.system('cp ../R-libraries/casal2_1.0.tar.gz Casal2/R-Libraries/casal2.tar.gz')
      
    if Globals.operating_system_ == "windows":
      os.system("zip -r Casal2.zip CASAL2/*")
    else:
      os.system('tar cvf Casal2.tar Casal2/')
      os.system('gzip Casal2.tar')
      os.system('mv Casal2.tar.gz ' + output_directory + 'Casal2.tar.gz')

    return True #Success!
