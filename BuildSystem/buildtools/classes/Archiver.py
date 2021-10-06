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
  do_build_ = ""
  def start(self, skip_building = 'false'):
    if Globals.operating_system_ == "windows":
      self.do_build_ = 'doBuild.bat'
    else:
      self.do_build_ = './doBuild.sh'

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
      if subprocess.call(self.do_build_ + ' thirdparty > third_party.log 2>&1', shell=True) != EX_OK:
        return Globals.PrintError('Failed to build the third party libraries. Please check third_party.log for the error')
      #subprocess.call('rm -rf third_party.log', shell=True)

      print('--> Building release version of Casal2 library')
      print('-- Re-Entering the build system to build a release library')
      print('-- All output is being diverted to release_build.log')
      if subprocess.call(self.do_build_ + ' library release > release_build.log 2>&1', shell=True) != EX_OK:
        return Globals.PrintError('Failed to build release library. Please check release_build.log for the error')
      #subprocess.call('rm -rf release_build.log', shell=True)

      print('--> Building release version of ADOL-C Casal2')
      print('-- Re-Entering the build system to build an adolc release library')
      print('-- All output is being diverted to release_adolc_build.log')
      if subprocess.call(self.do_build_ + ' library adolc > release_adolc_build.log 2>&1', shell=True) != EX_OK:
        return Globals.PrintError('Failed to build release library. Please check release_adolc_build.log for the error')
      subprocess.call('rm -rf release_adolc_build.log', shell=True)

      print('--> Building release version of BetaDiff Casal2')
      print('-- Re-Entering the build system to build a betadiff release library')
      print('-- All output is being diverted to release_betadiff_build.log')
      if subprocess.call(self.do_build_ + ' library betadiff > release_betadiff_build.log 2>&1', shell=True) != EX_OK:
        return Globals.PrintError('Failed to build release library. Please check release_betadiff_build.log for the error')
      subprocess.call('rm -rf release_betadiff_build.log', shell=True)

      # print('--> Building documentation')
      # print('-- Re-Entering the build system to build the documentation')
      # print('-- All output is being diverted to documentation_build.log')
      # if subprocess.call(self.do_build_ + ' documentation > documentation_build.log 2>&1', shell=True) != EX_OK:
      #   return Globals.PrintError('Failed to build the documentation. Please check documentation_build.log for error')
      # subprocess.call('rm -rf documentation_build.log', shell=True)

      print('--> Building test version of Casal2')
      print('-- Re-Entering the build system to build a unit test library')
      print('-- All output is being diverted to unit_test_build.log')
      if subprocess.call(self.do_build_ + ' library test > unit_test_build.log 2>&1', shell=True) != EX_OK:
        return Globals.PrintError('Failed to build release library. Please check unit_test_build.log for the error')
      subprocess.call('rm -rf unit_test_build.log', shell=True)

      print('--> Building Front End Binary')
      print('-- Re-Entering the build system to build the front end binary')
      print('-- All output is being diverted to front_end_build.log')
      if subprocess.call(self.do_build_ + ' frontend > front_end_build.log 2>&1', shell=True) != EX_OK:
        return Globals.PrintError('Failed to build the front end binary. Please check front_end_build.log for error')
      #subprocess.call('rm -rf front_end_build.log', shell=True)

    ## Now we actually do the zipping of the binary
    output_directory = "bin/" + Globals.operating_system_ + "/archive/"
    if not os.path.exists(output_directory):
      os.makedirs(output_directory);
    print("-- Target output directory: " + output_directory)

    if os.path.exists(output_directory + "Casal2.tar.gz"):
      print("-- Removing old Archive")
      subprocess.call("rm -rf " + output_directory + "Casal2.tar.gz", shell=True)

    # Shitty stuff to make building archive easier
    binary_path = self.output_directory_ = os.path.normpath(os.getcwd()) + "/bin/" + Globals.operating_system_ + "_" + Globals.compiler_

    subprocess.call('rm -rf Casal2', shell=True)
    subprocess.call('rm -rf Casal2.tar', shell=True)
    subprocess.call('rm -rf Casal2.zip', shell=True)
    os.makedirs('Casal2')
    os.makedirs('Casal2/R-Libraries')
    print("-- Directories created")
    subprocess.call('cp ' + binary_path + '/frontend/' + binary_name + ' Casal2/' + binary_name, shell=True)
    subprocess.call('cp ' + binary_path + '/library_release/' + library_name + ' Casal2/casal2_release' + extension, shell=True)
    subprocess.call('cp ' + binary_path + '/library_adolc/' + library_name + ' Casal2/casal2_adolc' + extension, shell=True)
    subprocess.call('cp ' + binary_path + '/library_betadiff/' + library_name + ' Casal2/casal2_betadiff' + extension, shell=True)
    subprocess.call('cp ' + binary_path + '/library_test/' + library_name + ' Casal2/casal2_test' + extension, shell=True)
    print("-- Binaries copied")
    subprocess.call('cp ../Documentation/UserManual/CASAL2.pdf Casal2/Casal2.pdf', shell=True)
    subprocess.call('cp ../Documentation/UserManual/CASAL2.xml Casal2/CASAL2.xml', shell=True)
    subprocess.call('cp ../Documentation/UserManual/Notepad++_syntax_highlighter.readme Casal2/Notepad++_syntax_highlighter.readme', shell=True)
    subprocess.call('cp ../Documentation/UserManual/CASAL2.syn Casal2/CASAL2.syn', shell=True)
    subprocess.call('cp ../Documentation/UserManual/TextPad_syntax_highlighter.readme Casal2/TextPad_syntax_highlighter.readme', shell=True)
    print("-- Manual copied")
    subprocess.call('cp -r ../Examples Casal2/Examples', shell=True)
    print("-- Example code copied")
    subprocess.call('cp -r ../README.txt Casal2/README.txt', shell=True)
    subprocess.call('cp -r ../README.md Casal2/README.md', shell=True)
    print("-- Readme copied")
    subprocess.call('cp -r ../LICENSE Casal2/LICENSE', shell=True)
    print("-- license copied")
    ## deal with R libraries
    subprocess.call('cp ../R-libraries/Casal2_' + Globals.Casal2_version_number + '.tar.gz Casal2/R-Libraries/Casal2.tar.gz', shell=True)
    print("-- R libraries copied")

    if Globals.operating_system_ == "windows":
      subprocess.call("zip -r Casal2.zip CASAL2/*", shell=True)
    else:
      subprocess.call('tar cvf Casal2.tar Casal2/', shell=True)
      subprocess.call('gzip Casal2.tar', shell=True)
      subprocess.call('mv Casal2.tar.gz ' + output_directory + 'Casal2.tar.gz', shell=True)

    return True #Success!
