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
  
  def start(self):
    print "--> Archiving any previously built binaries"
    print "--> Checking if Third Party libraries have been built"
    third_party_dir = "bin/" + Globals.operating_system_ + "/thirdparty"
    if not os.path.exists(third_party_dir):
      return Globals.PrintError("Please build third party libraries before you build archive")

    print "--> Checking for release version of iSAM binary"
    binary_path = self.output_directory_ = os.path.normpath(os.getcwd()) + "/bin/" + Globals.operating_system_ + "/release/isam"
    if Globals.operating_system_ == "win32":
      binary_path += ".exe"
    if not os.path.exists(binary_path):
      print "-- No iSAM binary was found. Buiding one now"
      Globals.build_target_ = "release"
      builder = MainCode()
      if not builder.start():
        return False

    output_directory = "bin/" + Globals.operating_system_ + "/archive/"
    if not os.path.exists(output_directory):
      os.makedirs(output_directory);
    print "-- Target output directory: " + output_directory

    if os.path.exists(output_directory + "isam.zip"):
      print "-- Removing old Zip Archive"
      os.system("rm -rf " + output_directory + "isam.zip")

    print "-- Creating new zip archive"
    if os.system("zip -j " + output_directory + "isam.zip " + binary_path) != EX_OK:
      return Globals.PrintError("Failed to create zip archive")
    
    return True #Success!
    
