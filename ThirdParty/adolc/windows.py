import os
import shutil
import time
from distutils import dir_util

import Globals

class Builder:
  version_ = 2.5
    
  def start(self):
    # Variables
    fileName         = 'msys'    
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
    os.system('rm -rf ' + Globals.target_debug_lib_path_ + library)
    os.system('rm -rf ' + Globals.target_release_lib_path_ + library)        
        
    # Decompress our archive
    print '-- Decompressing - check isam_unzip.log'
    if os.path.exists(fileName + '.zip'):
      os.system('unzip ' + fileName + '.zip 1> isam_unzip.log 2>&1')
    
    # Build
    os.chdir(fileName + '\\1.0\\')
    print '-- Starting msys to decompress and build the ADOLC libary'
    os.system("msys.bat --norxvt")

    count = 0
    while not os.path.exists('home/Admin/ADOL-C-2.5.2/ADOL-C/.libs/libadolc.a'):
      count += 1
      if count > 600:
        print '-- Timeout exceeded while waiting for msys to compile ADOL-C'
        return False
      if count % 5 == 0:
        print '-- Waiting for msys compilation to finish'
      time.sleep(5);
   
    # Move our headers and libraries
    print '-- Moving headers and libraries'
    os.chdir('home/Admin/ADOL-C-2.5.2/ADOL-C/')
    dir_util.copy_tree('include/' + headers, Globals.target_include_path_ + '/adolc/')
    shutil.copy('.libs/' + library, Globals.target_debug_lib_path_)
    shutil.copy('.libs/' + library, Globals.target_release_lib_path_)    
     
    return True
