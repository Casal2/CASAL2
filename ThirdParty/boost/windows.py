import os
import os.path
import shutil
from distutils import dir_util

import Globals

class Builder:
  version_ = '1_55'
  
  def start(self):    
    # Variables
    boostFileName   = 'boost_1_55_0'
    
    # Figure out GCC version
    pieces = Globals.compiler_version_.split('.')
    if len(pieces) != 3:
      return Globals.PrintError('Boost could not parse GCC Version properly: ' + Globals.compiler_version_)
    
    gcc_version = str(pieces[0]) + str(pieces[1])
    print '-- GCC Version for Boost: ' + gcc_version
  
    # organise the libraries we want to copy over
    debug_libraries = [ 'libboost_program_options-mgw'+gcc_version+'-mt-sd-'+self.version_+'.a',
                'libboost_system-mgw'+gcc_version+'-mt-sd-'+self.version_+'.a',
                'libboost_thread-mgw'+gcc_version+'-mt-sd-'+self.version_+'.a' ]
    release_libraries = [ 'libboost_program_options-mgw'+gcc_version+'-mt-s-'+self.version_+'.a',
                'libboost_system-mgw'+gcc_version+'-mt-s-'+self.version_+'.a',
                'libboost_thread-mgw'+gcc_version+'-mt-s-'+self.version_+'.a' ]  
  
    # Clean our any existing files if they already exist
    print '-- Cleaning Boost files'
    if os.path.exists(boostFileName):
        shutil.rmtree(boostFileName)
    if os.path.exists(Globals.target_include_path_ + 'boost'):
        shutil.rmtree(Globals.target_include_path_ + 'boost')
    for library in debug_libraries:
        os.system('rm -rf ' + Globals.target_debug_lib_path_ + library)
    for library in release_libraries:
        os.system('rm -rf ' + Globals.target_release_lib_path_ + library)
    
    # Decompress the boost archive if it hasn't already been done
    if not os.path.exists(boostFileName):       
        # Decompress our boost archive
        print '-- Decompressing Boost'
        if os.path.exists(boostFileName + '_windows.zip'):
            os.system('unzip ' + boostFileName + '_windows.zip 1> isam_unzip.log 2>&1')      
    
    # Build Boost
    print '-- Building Boost - check isam_build.log for progress - estimated time 30 minutes'
    os.chdir(boostFileName)
    os.system('bootstrap.bat gcc 1> isam_bootstrap.log 2>&1')
    os.system('b2.exe --toolset=gcc runtime-link=static threading=multi 1> isam_build.log 2>&1')
        
    # Move our headers and libraries
    print '-- Moving headers and libraries'
    dir_util.copy_tree('boost', Globals.target_include_path_ + '/boost/')
    for library in debug_libraries:
      shutil.copy('stage/lib/' + library, Globals.target_debug_lib_path_)
    for library in release_libraries:
      shutil.copy('stage/lib/' + library, Globals.target_release_lib_path_)
    
    return True
