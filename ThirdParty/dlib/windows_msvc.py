import os
import os.path
import subprocess
import sys
import shutil
from distutils import dir_util

import Globals

class Builder:
  version_ = '19.17'
  
  def is_lean(self):
    return True
  
  def start(self):    
    # Variables
    fileName        = 'dlib-19.17'

    # Figure out MSVC version
    pieces = Globals.compiler_version_.split('.')
    if len(pieces) != 4:
      return Globals.PrintError('Boost could not parse Microsoft Visual C++ Version properly: ' + Globals.compiler_version_)
    
    msvc_version = pieces[0] + pieces[1]
    print('-- Microsoft Visual C++ Version for Boost: ' + msvc_version)

    # Clean our any existing files if they already exist
    print('-- Cleaning DLib files')
    if os.path.exists(fileName):
      shutil.rmtree(fileName)
    if os.path.exists(Globals.target_include_path_ + 'dlib'):
      shutil.rmtree(Globals.target_include_path_ + 'dlib')

    os.system('rm -rf ' + Globals.target_debug_lib_path_ + 'libdlib.lib')
    os.system('rm -rf ' + Globals.target_debug_lib_path_ + 'libdlib.lib')

    # Decompress our archive
    print('-- Decompressing DLib - check casal2_unzip.log')
    if os.path.exists(fileName + '.zip'):
      os.system('unzip ' + fileName + '.zip 1> casal2_unzip.log 2>&1')

    # configure MSVC specific stuff
    os.chdir(fileName)
    os.system('cmake . 1> casal2_cmake.log 2>&1')
    os.system('devenv Project.sln /Build Release 2> casal2_make.log 2>&1')
    
    # Move our headers and libraries
    print('-- Moving headers')
    dir_util.copy_tree('dlib', Globals.target_include_path_ + 'dlib')
    shutil.copy('dlib/Release/dlib19.17.0_release_64bit_msvc' + msvc_version + '.lib', Globals.target_debug_lib_path_)
    shutil.copy('dlib/Release/dlib19.17.0_release_64bit_msvc' + msvc_version + '.lib', Globals.target_release_lib_path_)    
        
    return True
