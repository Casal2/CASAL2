import os
import os.path
import subprocess
import sys
import shutil
from distutils import dir_util

import Globals

class Builder:
  version_ = '19.17'

  def is_lean(self):
    return True
  
  def start(self):    
    # Variables
    fileName        = 'dlib-19.17'
  
    # Clean our any existing files if they already exist
    print('-- Cleaning DLib files')
    if os.path.exists(fileName):
      shutil.rmtree(fileName)
    if os.path.exists(Globals.target_include_path_ + 'dlib'):
      shutil.rmtree(Globals.target_include_path_ + 'dlib')

    os.system('rm -rf ' + Globals.target_debug_lib_path_ + 'libdlib.a')
    os.system('rm -rf ' + Globals.target_debug_lib_path_ + 'libdlib.a')

    # Decompress our archive
    print('-- Decompressing DLib - check casal2_unzip.log')
    if os.path.exists(fileName + '.zip'):
      os.system('unzip ' + fileName + '.zip 1> casal2_unzip.log 2>&1')
    
    # configure linux specific stuff
    os.chdir(fileName)
    os.system('cmake -G "Unix Makefiles" 1> casal2_cmake.log 2>&1')
    os.system('make 2> casal2_make.log 2>&1')

    # Move our headers and libraries
    print('-- Moving headers')
    dir_util.copy_tree('dlib', Globals.target_include_path_ + 'dlib')
    shutil.copy('dlib/libdlib.a', Globals.target_debug_lib_path_)
    shutil.copy('dlib/libdlib.a', Globals.target_release_lib_path_)    
        
    return True
