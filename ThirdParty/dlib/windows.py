import os
import os.path
import subprocess
import sys
import shutil
from distutils import dir_util

import Globals

class Builder:
  version_ = '18.16'
  
  def is_lean(self):
    return True
  
  def start(self):    
    # Variables
    fileName        = 'dlib-18.16'
  
    # Clean our any existing files if they already exist
    print('-- Cleaning DLib files')
    if os.path.exists(fileName):
      shutil.rmtree(fileName)
    if os.path.exists(Globals.target_include_path_ + 'dlib'):
      shutil.rmtree(Globals.target_include_path_ + 'dlib')
    # Decompress our archive
    print('-- Decompressing DLib - check casal2_unzip.log')
    if os.path.exists(fileName + '.zip'):
      os.system('unzip ' + fileName + '.zip 1> casal2_unzip.log 2>&1')
    
    # Move our headers and libraries
    print('-- Moving headers')
    dir_util.copy_tree(fileName + '/dlib', Globals.target_include_path_ + 'dlib')
        
    return True
