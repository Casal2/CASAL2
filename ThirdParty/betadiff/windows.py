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
    print('-- Cleaning files'  )
    if os.path.exists(fileName):
      shutil.rmtree(fileName)
    print('-- Removing previously built headers and libraries')
    os.system('rm -rf ' + Globals.target_include_path_ + "ADOLC18")
    os.system('rm -rf ' + Globals.target_include_path_ + header)
    os.system('rm -rf ' + Globals.target_special_lib_path_ + library)        
        
    # Decompress our archive
    print('-- Decompressing - check casal2_unzip.log')
    if os.path.exists(fileName + '.zip'):
      os.system('unzip ' + fileName + '.zip 1> casal2_unzip.log 2>&1')
  
    # Run CMake
    print('-- Configuring cmake - check ' + fileName + '/casal2_cmake.log')
    os.chdir(fileName)
    os.system('cmake -G "MinGW Makefiles" 1> casal2_cmake.log 2>&1')
  
    # Build 
    print('-- Building - check ' + fileName + '/casal2_build.log')
    os.system("mingw32-make 1> casal2_build.log 2>&1")
   
    # Move our headers and libraries
    print('-- Moving headers and libraries')
    dir_util.copy_tree('ADOLC18/', Globals.target_include_path_ + '/ADOLC18/')
    shutil.copy(header, Globals.target_include_path_)
    shutil.copy(library, Globals.target_special_lib_path_)    
     
    return True
