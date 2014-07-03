import os
import os.path
import shutil
from distutils import dir_util

import Globals

def doBuild():
  
  # Variables
  version         = 1.55
  boostFileName   = 'boost_1_55_0'
  targetPath      = Globals.target_path_

  # Figure out GCC version
  pieces = Globals.compiler_version_.split('.')
  if len(pieces) != 3:
    return Globals.PrintError('Boost could not parse GCC Version properly: ' + Globals.compiler_version_)
  
  gcc_version = str(pieces[0]) + str(pieces[1])
  print '--> GCC Version for Boost: ' + gcc_version

  # Clean our any existing files if they already exist
  print '-- Cleaning Boost files'
  if os.path.exists(boostFileName):
      shutil.rmtree(boostFileName)
  
  if not os.path.exists(boostFileName):       
      # Decompress our boost archive
      print '-- Decompressing Boost'
      if os.path.exists(boostFileName + '_windows.zip'):
          os.system('unzip ' + boostFileName + '_windows.zip 1> isam_unzip.log 2>&1')      
  
  # Build Boost
  print '-- Building Boost - check isam_build.log for progress - estimated time 30 minutes'
  os.chdir(boostFileName)
  os.system('bootstrap.bat gcc 1> isam_bootstrap.log 2>&1')
  os.system('b2.exe --toolset=gcc runtime-link=static threading=multi 1> isam_build.log 2>&1')
      
  # Move our headers and libraries
  print '-- Moving headers and libraries to ' + targetPath
  dir_util.copy_tree('boost', targetPath + '/include/boost')
  shutil.copy('stage/lib/libboost_program_options-mgw'+gcc_version+'-mt-s-1_55.a', targetPath + "/lib/")
  shutil.copy('stage/lib/libboost_system-mgw'+gcc_version+'-mt-s-1_55.a', targetPath + "/lib/")
  shutil.copy('stage/lib/libboost_thread-mgw'+gcc_version+'-mt-s-1_55.a', targetPath + "/lib/")
  
  os.system('touch ' + targetPath + '/boost.success')
  return True
