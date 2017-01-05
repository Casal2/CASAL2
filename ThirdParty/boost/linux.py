import os
import os.path
import shutil
from distutils import dir_util

import Globals

class Builder:
  version_ = '1_58'

  def is_lean(self):
    return True
  
  def start(self):
      # Variables
      boostFileName = 'boost_1_58_0'
      
      libraries = [ 'libboost_program_options.a',
                   'libboost_system.a', 
                   'libboost_thread.a' ]      
  
      # Clean our any existing files if they already exist
      print '-- Cleaning Boost files'
      if os.path.exists(boostFileName):
        shutil.rmtree(boostFileName)
      if os.path.exists(Globals.target_include_path_ + 'boost'):
        shutil.rmtree(Globals.target_include_path_ + 'boost')
      for library in libraries:
        os.system('rm -rf ' + Globals.target_debug_lib_path_ + library)
        os.system('rm -rf ' + Globals.target_release_lib_path_ + library)        
  
      # Make a copy of boost.tar.gz as temp.tar.gz if one doesn't exist
      if not os.path.exists('temp.tar') and not os.path.exists('temp.tar.gz'):
        print '-- Copying Boost archive'
        os.system('cp ' + boostFileName + '.tar.gz temp.tar.gz')
  
      # Decompress our boost archive
      print '-- Decompressing Boost'
      if os.path.exists('temp.tar.gz'):
        os.system('gzip -d temp.tar.gz')
  
      if os.path.exists('temp.tar'):
        os.system('tar -x -ftemp.tar')
        os.system('rm -rf temp.tar')
  
      # Build Boost
      print '-- Building Boost - check casal2_build.log for progress - estimated time 30 minutes'
      os.chdir(boostFileName)
  
      os.system('./bootstrap.sh gcc 1> casal2_bootstrap.log 2>&1')
      os.system("./b2 address-model=64 architecture=x86 --toolset=gcc link=static runtime-link=static threading=multi cxxflags='-fPIC' 1> casal2_build.log 2>&1")
                
      # Move our headers and libraries
      print '-- Moving headers and libraries'
      dir_util.copy_tree('boost', Globals.target_include_path_ + '/boost/')
      for library in libraries:
        shutil.copy('stage/lib/' + library, Globals.target_debug_lib_path_)
        shutil.copy('stage/lib/' + library, Globals.target_release_lib_path_)
      
      return True
