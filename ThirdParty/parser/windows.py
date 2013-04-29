import os
import os.path
import subprocess
import sys
import shutil

import Globals

def doBuild():
  # Variables
  fileName = 'parser'
  targetPath      = Globals.target_path_
  
  # Clean our any existing files if they already exist
  print '-- Cleaning Parser files'
  if os.path.exists(fileName):
      shutil.rmtree(fileName)
  
  # Decompress our archive
  print '-- Decompressing Parser - check isam_unzip.log'
  if os.path.exists(fileName + '.zip'):
      os.system('unzip ' + fileName + '.zip 1> isam_unzip.log 2>&1')
  
  # Build 
  print '-- Building Parser - check parser/isam_build.log'
  os.chdir(fileName)
  os.system("g++ -g0 -c parser.cpp 1> isam_build.log 2>&1")
  os.system("ar rvs parser.a parser.o 1>> isam_build.log 2>&1")
      
  # Move our headers and libraries
  print '-- Moving headers and libraries'
  shutil.copy('parser.a', targetPath + "/lib/")
  shutil.copy('parser.h', targetPath + "/include/")
  
  # Finally touch the success file to create it
  os.system('touch ' + targetPath + '/parser.success')