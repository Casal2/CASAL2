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
    boostFileName   = 'boost_1_71_0'
    
    # Figure out MSVC version
    pieces = Globals.compiler_version_.split('.')
    if len(pieces) != 4:
      return Globals.PrintError('Boost could not parse Microsoft Visual C++ Version properly: ' + Globals.compiler_version_)
    
    msvc_version = pieces[0]
    print('-- Microsoft Visual C++ Version for Boost: ' + msvc_version)
    library_version = ""
    if msvc_version == "19":
        library_version = "142"
  
    # organise the libraries we want to copy over
    libraries = [ 'libboost_program_options-vc',
                'libboost_system-vc' ]
    
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
            os.system('7za.exe x -bd -y ' + boostFileName + '.7z 1> casal2_unzip.log 2>&1')      
    
    # Build Boost
    print('-- Building Boost - check casal2_build.log for progress')
    os.chdir(boostFileName)
    os.system('bootstrap.bat 1> casal2_bootstrap.log 2>&1')
    os.system('b2.exe link=static runtime-link=static threading=multi --with-system --with-program_options --with-python stage 1> casal2_build.log 2>&1')
        
    # Move our headers and libraries
    print('-- Moving headers and libraries')
    dir_util.copy_tree('boost', Globals.target_include_path_ + '/boost/')
    for library in libraries:
      shutil.copy('stage/lib/' + library+library_version+'-mt-sgd-x64-'+self.version_+'.lib', Globals.target_debug_lib_path_)
      shutil.copy('stage/lib/' + library+library_version+'-mt-s-x64-'+self.version_+'.lib', Globals.target_release_lib_path_)
    
    return True
