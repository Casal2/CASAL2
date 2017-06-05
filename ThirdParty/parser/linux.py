import os
import os.path
import subprocess
import sys
import shutil

import Globals

class Builder:
  version_ = '1.1'
  
  def is_lean(self):
    return True  
  
  def start(self):
    # Variables
    fileName = 'parser'
    header   = 'parser.h'
    header2   = 'parser-inl.h'
    library  = 'parser.a'
    
    # Clean our any existing files if they already exist
    print '-- Cleaning Parser files'
    if os.path.exists(fileName):
      shutil.rmtree(fileName)
    os.system('rm -rf ' + Globals.target_include_path_ + header)
    os.system('rm -rf ' + Globals.target_include_path_ + header2)
    os.system('rm -rf ' + Globals.target_debug_lib_path_ + library)
    os.system('rm -rf ' + Globals.target_release_lib_path_ + library)        
    
    # Decompress our archive
    print '-- Decompressing Parser - check casal2_unzip.log'
    if os.path.exists(fileName + '.zip'):
      os.system('unzip ' + fileName + '.zip 1> casal2_unzip.log 2>&1')
    
    # Build     
    os.chdir(fileName)
            
    # Move our headers and libraries
    print '-- Moving headers and libraries'
    shutil.copy(header, Globals.target_include_path_)
    shutil.copy(header2, Globals.target_include_path_)
        
    return True
