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
    boostFileName   = 'boost_1_58_0'
    
    # Figure out GCC version
    pieces = Globals.compiler_version_.split('.')
    if len(pieces) != 3:
      return Globals.PrintError('Boost could not parse GCC Version properly: ' + Globals.compiler_version_)
    
    gcc_version = str(pieces[0]) + str(pieces[1])
    print '-- GCC Version for Boost: ' + gcc_version
  
    # organise the libraries we want to copy over
    libraries = [ 'libboost_program_options-mgw',
                'libboost_system-mgw',
                'libboost_thread-mgw' ]
    
    # Clean our any existing files if they already exist
    print '-- Cleaning Boost files'
    if os.path.exists(boostFileName):
        shutil.rmtree(boostFileName)
    if os.path.exists(Globals.target_include_path_ + 'boost'):
        shutil.rmtree(Globals.target_include_path_ + 'boost')
    for library in libraries:
        os.system('rm -rf ' + Globals.target_debug_lib_path_ + library + '*')
        os.system('rm -rf ' + Globals.target_release_lib_path_ + library + '*')
    
    # Decompress the boost archive if it hasn't already been done
    if not os.path.exists(boostFileName):       
        # Decompress our boost archive
        print '-- Decompressing Boost'
        if os.path.exists(boostFileName + '_windows.zip'):
            os.system('unzip ' + boostFileName + '_windows.zip 1> casal2_unzip.log 2>&1')      
    
    # Build Boost
    print '-- Building Boost - check casal2_build.log for progress - estimated time 30 minutes'
    os.chdir(boostFileName)
    os.system('bootstrap.bat gcc 1> casal2_bootstrap.log 2>&1')
    os.system('b2.exe --toolset=gcc runtime-link=static threading=multi 1> casal2_build.log 2>&1')
        
    # Move our headers and libraries
    print '-- Moving headers and libraries'
    dir_util.copy_tree('boost', Globals.target_include_path_ + '/boost/')
    for library in libraries:
      shutil.copy('stage/lib/' + library+gcc_version+'-mt-sd-'+self.version_+'.a', Globals.target_debug_lib_path_)
      shutil.copy('stage/lib/' + library+gcc_version+'-mt-s-'+self.version_+'.a', Globals.target_release_lib_path_)
    
    return True
