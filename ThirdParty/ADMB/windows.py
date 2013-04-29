import os
import os.path
import subprocess
import sys
import shutil
import fileinput
from distutils import dir_util

import Globals

def doBuild():
  admbFileName    = 'admb-11'
  targetPath      = Globals.target_path_

  print '-- Cleaning existing files'
  if os.path.exists(admbFileName):
    shutil.rmtree(admbFileName)
  if os.path.exists("unzip.log"):
    os.remove("unzip.log")
  
  print '-- Decompressing archive - check unzip.log'  
  if os.path.exists(admbFileName + '.zip'):
    os.system('unzip ' + admbFileName + '.zip 1> unzip.log 2>&1')
  else:
    return Globals.PrintError("Archie " + admbFileName + ".zip does not exist.")
  
  # Build ADMB
  print '-- Building ADMB - check ' + admbFileName + '/make.log for progress - estimated time 30-60 minutes'
  if (not os.path.exists(admbFileName)):
    return Globals.PrintError("Folder " + admbFileName + " does not exist after decompressing archive")
  os.chdir(admbFileName)
  
  os.system("mingw32-make -j4 1> make.log 2>&1")  
  if not os.path.exists('build/mingw/include') or not os.path.exists('build/mingw/lib'):
    return Globals.PrintError('ADMB failed to compile. Please check log file for error')
            
  # Move our headers and libraries
  print '-- Moving headers and libraries'
  dir_util.copy_tree('build/mingw/include', targetPath + '/include/admb/')
  dir_util.copy_tree('build/mingw/lib', targetPath + '/lib')
      
  # Finally touch the success file to create it
  os.system('touch ' + targetPath + '/ADMB.success')
  return True