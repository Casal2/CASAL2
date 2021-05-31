import os
import os.path
import shutil
from distutils import dir_util

import Globals

class Builder:
  version_ = '1_71'

  def is_lean(self):
    return True
  
  def start(self):
    # Variables
    boostFileName = 'boost_1_71_0'
    
    libraries = [ 'libboost_program_options.a',
                 'libboost_system.a', 
                ]      

    # Clean our any existing files if they already exist
    print('-- Cleaning Boost files')
    if os.path.exists(boostFileName):
      shutil.rmtree(boostFileName)
    if os.path.exists(Globals.target_include_path_ + 'boost'):
      shutil.rmtree(Globals.target_include_path_ + 'boost')
    os.system('rm -rf ' + Globals.target_debug_lib_path_ + 'libboost_*')
    os.system('rm -rf ' + Globals.target_release_lib_path_ + 'libboost_*')     
    
    # Decompress the boost archive if it hasn't already been done
    if not os.path.exists(boostFileName):       
        # Decompress our boost archive
        print('-- Decompressing Boost - estimated time 5 minutes')
        if os.path.exists(boostFileName + '.7z'):
            os.system('7za x -bd -y ' + boostFileName + '.7z 1> casal2_unzip.log 2>&1')     

    # Build Boost
    print('-- Building Boost - check casal2_build.log for progress')
    os.chdir(boostFileName)

    os.system('./bootstrap.sh gcc 1> casal2_bootstrap.log 2>&1')
    os.system("./b2 address-model=64 architecture=x86 --toolset=gcc link=static threading=multi cxxflags='-fPIC' --with-system --with-program_options stage 1> casal2_build.log 2>&1")
              
    # Move our headers and libraries
    print('-- Moving headers and libraries')
    dir_util.copy_tree('boost', Globals.target_include_path_ + '/boost/')
    for library in libraries:
      shutil.copy('stage/lib/' + library, Globals.target_debug_lib_path_)
      shutil.copy('stage/lib/' + library, Globals.target_release_lib_path_)
    
    return True
