import os
import shutil
from distutils import dir_util

import Globals

def doBuild():
  return True
"""
  # Variables
  gmockFileName = 'gmock-1.6.0'
  targetPath      = Globals.target_path_
  operatingSystem = Globals.operating_system_
  
  # Clean our any existing files if they already exist
  print '-- Cleaning files'
  if os.path.exists(gmockFileName):
      shutil.rmtree(gmockFileName)
      
  # Decompress our archive
  print '-- Decompressing - check isam_unzip.log'
  if os.path.exists(gmockFileName + '.zip'):
      os.system('unzip ' + gmockFileName + '.zip 1> isam_unzip.log 2>&1')
  
  # Configure The Build
  if (operatingSystem == 'linux'):
      cwd = os.path.normpath(os.getcwd())
      os.chdir(gmockFileName)
      os.system("./configure 1> isam_configure.log 2>&1")    
      os.chdir(cwd)
 
  # Build 
  print '-- Building - check ' + gmockFileName +'/make/isam_build.log'
  os.chdir(gmockFileName + '/make')
  os.system("make 1> isam_build.log 2>&1")
      
  # Move our headers and libraries
  print '-- Moving headers and libraries'
  shutil.copy('gmock_main.a', targetPath + "/lib/")
  dir_util.copy_tree('../include/', targetPath + '/include/')
  dir_util.copy_tree('../gtest/include/', targetPath + '/include/')
  
  # Finally touch the success file to create it
  os.system('touch ' + targetPath + '/googletest_googlemock.success')
  return True"""