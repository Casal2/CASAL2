import os
import shutil
from distutils import dir_util

import Globals

class Builder:
  version_ = '1.6.0'

  def is_lean(self):
    return True
  
  def start(self):
    # Variables
    gmockFileName = 'gmock-1.6.0'
    library = 'gmock_main.a'
    
    # Clean our any existing files if they already exist
    print '-- Cleaning files'
    if os.path.exists(gmockFileName):
        shutil.rmtree(gmockFileName)
    if os.path.exists(Globals.target_include_path_ + 'gmock'):
      shutil.rmtree(Globals.target_include_path_ + 'gmock')
    if os.path.exists(Globals.target_include_path_ + 'gtest'):
      shutil.rmtree(Globals.target_include_path_ + 'gtest')
    os.system('rm -rf ' + Globals.target_debug_lib_path_ + library)
    os.system('rm -rf ' + Globals.target_release_lib_path_ + library)
        
    # Decompress our archive
    print '-- Decompressing - check casal2_unzip.log'
    if os.path.exists(gmockFileName + '.zip'):
        os.system('unzip ' + gmockFileName + '.zip 1> casal2_unzip.log 2>&1')
        
    # configure linux specific stuff
    cwd = os.path.normpath(os.getcwd())
    os.chdir(gmockFileName)
    os.system("chmod +x configure")    
    os.system("./configure 1> casal2_configure.log 2>&1")    
    os.chdir(cwd)
   
    # Build 
    print '-- Building - check ' + gmockFileName +'/make/casal2_build.log'
    os.chdir(gmockFileName + '/make')
    os.system("make CXXFLAGS='-fPIC' GTEST_CXXFLAGS='-fPIC' 1> casal2_build.log 2>&1")
        
    # Move our headers and libraries
    print '-- Moving headers and libraries'
    shutil.copy('gmock_main.a', Globals.target_debug_lib_path_)
    shutil.copy('gmock_main.a', Globals.target_release_lib_path_)
    dir_util.copy_tree('../include/', Globals.target_include_path_)
    dir_util.copy_tree('../gtest/include/', Globals.target_include_path_)
    
    return True
