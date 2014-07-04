import os
import shutil
from distutils import dir_util

import Globals

class Builder:
  version_ = 1.0
  
  def start(self):
    # Variables
    fileName         = 'betadiff'    
    operatingSystem  = Globals.operating_system_
    
    header  = 'betadiff.h'
    library = 'libbetadiff.a'
    
    # Clean our any existing files if they already exist
    print '-- Cleaning files'  
    if os.path.exists(fileName):
      shutil.rmtree(fileName)
    print '-- Removing previously built headers and libraries'
    os.system('rm -rf ' + Globals.target_include_path_ + header)
    os.system('rm -rf ' + Globals.target_debug_lib_path_ + library)
    os.system('rm -rf ' + Globals.target_release_lib_path_ + library)        
        
    # Decompress our archive
    print '-- Decompressing - check isam_unzip.log'
    if os.path.exists(fileName + '.zip'):
      os.system('unzip ' + fileName + '.zip 1> isam_unzip.log 2>&1')
  
    # Run CMake
    print '-- Configuring cmake - check ' + fileName + '/isam_cmake.log'
    os.chdir(fileName)
    os.system('cmake -G "Unix Makefiles" 1> isam_cmake.log 2>&1')
  
    # Build 
    print '-- Building - check ' + fileName + '/isam_build.log'
    os.system("make 1> isam_build.log 2>&1")
   
    # Move our headers and libraries
    print '-- Moving headers and libraries'
    shutil.copy(header, Globals.target_include_path_)
    shutil.copy(library, Globals.target_debug_lib_path_)
    shutil.copy(library, Globals.target_release_lib_path_)    
     
    return True