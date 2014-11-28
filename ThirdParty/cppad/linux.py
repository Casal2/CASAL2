import os
import shutil
from distutils import dir_util

import Globals

class Builder:
  version_ = 2.52
  
  def start(self):     
    # Variables
    fileName         = 'ADOL-C-2.5.2'    
    operatingSystem  = Globals.operating_system_
    
    headers  = 'adolc'
    library = 'libadolc.a'
    
    # Clean our any existing files if they already exist
    print '-- Cleaning files'  
    if os.path.exists(fileName):
      os.system('chmod 777 -R ' + fileName)
      shutil.rmtree(fileName)

    print '-- Removing previously built headers and libraries'
    os.system('rm -rf ' + Globals.target_include_path_ + headers)
    os.system('rm -rf ' + Globals.target_special_lib_path_ + library)
    
    # Decompress our archive
    print '-- Decompressing - check isam_unzip.log'
    if os.path.exists(fileName + '.zip'):
      os.system('unzip ' + fileName + '.zip 1> isam_unzip.log 2>&1')

    # Build
    os.chdir(fileName)
    print '-- Configuring - check isam_configure.log'
    os.system("./configure 1> isam_configure.log 2>&1")
    print '-- Building - check isam_build.log'
    os.system("make 1> isam_build.log 2>&1")

    # Move our headers and libraries
    print '-- Moving headers and libraries'
    os.chdir('ADOL-C/')
    dir_util.copy_tree('include/' + headers, Globals.target_include_path_ + '/adolc/')
    shutil.copy('.libs/' + library, Globals.target_special_lib_path_)

    return True
    
