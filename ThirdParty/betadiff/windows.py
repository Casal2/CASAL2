import os
import shutil
from distutils import dir_util

import Globals

def doBuild():
  # Variables
  fileName         = 'betadiff'
  targetPath       = Globals.target_path_
  operatingSystem  = Globals.operating_system_
  
  # Clean our any existing files if they already exist
  print '-- Cleaning files'
  if os.path.exists(fileName):
    shutil.rmtree(fileName)
      
  # Decompress our archive
  print '-- Decompressing - check isam_unzip.log'
  if os.path.exists(fileName + '.zip'):
    os.system('unzip ' + fileName + '.zip 1> isam_unzip.log 2>&1')

  # Run CMake
  print '-- Configuring cmake - check ' + fileName + '/isam_cmake.log'
  os.chdir(fileName)
  os.system('cmake -G "MinGW Makefiles" 1> isam_cmake.log 2>&1')

  # Build 
  print '-- Building - check ' + fileName + '/isam_build.log'
  os.system("make 1> isam_build.log 2>&1")
 
  # Move our headers and libraries
  print '-- Moving headers and libraries'
  shutil.copy('libbetadiff.a', targetPath + "/lib/")
  #dir_util.copy_tree('../include/', targetPath + '/include/')
  #dir_util.copy_tree('../gtest/include/', targetPath + '/include/')
  
  # Finally touch the success file to create it
  os.system('touch ' + targetPath + '/betadiff.success') 
  return True