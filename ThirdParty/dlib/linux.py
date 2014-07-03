import os
import os.path
import subprocess
import sys
import shutil
from distutils import dir_util

import Globals

def doBuild():
  
  # Variables
  fileName        = 'dlib-18.6'
  targetPath      = Globals.target_path_

  # Clean our any existing files if they already exist
  print '-- Cleaning DLib files'
  if os.path.exists(fileName):
      shutil.rmtree(fileName)
  
  # Decompress our archive
  print '-- Decompressing Parser - check isam_unzip.log'
  if os.path.exists(fileName + '.zip'):
      os.system('unzip ' + fileName + '.zip 1> isam_unzip.log 2>&1')
  
  # Move our headers and libraries
  print '-- Moving headers and libraries'
  dir_util.copy_tree(fileName + '/dlib', targetPath + '/include/dlib')
  
  # Finally touch the success file to create it
  os.system('touch ' + targetPath + '/dlib.success')
  return True