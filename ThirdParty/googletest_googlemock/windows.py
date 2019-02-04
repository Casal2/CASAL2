import os
import shutil
from distutils import dir_util

import Globals

class Builder:
  version_ = '1.8.1'

  def is_lean(self):
    return True
  
  def start(self):
    # Variables
    gmockFileName = 'googletest-release-1.8.1'
    libraries = [ 
      'gmock_main.a',
      'libgmock_main.a',
      'libgtest.a',
      'libgmock.a',
      '02libgmock.a',
      '01libgtest.a'
      ]
    
    # Clean our any existing files if they already exist
    print '-- Cleaning files'
    if os.path.exists(gmockFileName):
        shutil.rmtree(gmockFileName)
    if os.path.exists(Globals.target_include_path_ + 'gmock'):
      shutil.rmtree(Globals.target_include_path_ + 'gmock')
    if os.path.exists(Globals.target_include_path_ + 'gtest'):
      shutil.rmtree(Globals.target_include_path_ + 'gtest')
    for library in libraries:
      os.system('rm -rf ' + Globals.target_debug_lib_path_ + library + '*')
      os.system('rm -rf ' + Globals.target_release_lib_path_ + library + '*')  
    
    # Decompress our archive
    print '-- Decompressing - check casal2_unzip.log'
    if os.path.exists(gmockFileName + '.zip'):
        os.system('unzip ' + gmockFileName + '.zip 1> casal2_unzip.log 2>&1')
   
    # Build 
    print '-- Building - check ' + gmockFileName +'/make/casal2_build.log'
    os.chdir(gmockFileName + '/googlemock')
    os.system('cmake -G "MinGW Makefiles"')
    os.system("mingw32-make 1> casal2_build.log 2>&1")
        
    # Move our headers and libraries
    print '-- Moving headers and libraries'
    shutil.copy('libgmock.a', Globals.target_debug_lib_path_ + '/02libgmock.a')
    shutil.copy('libgmock.a', Globals.target_release_lib_path_ + '/02libgmock.a')
    shutil.copy('gtest/libgtest.a', Globals.target_debug_lib_path_ + '/01libgtest.a')
    shutil.copy('gtest/libgtest.a', Globals.target_release_lib_path_ + '/01libgtest.a')
    dir_util.copy_tree('include/', Globals.target_include_path_)
    dir_util.copy_tree('../googletest/include/', Globals.target_include_path_)
    
    return True
