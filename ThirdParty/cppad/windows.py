import os
import shutil
import time
from distutils import dir_util

import Globals

class Builder:
  version_ = 3.11
    
  def start(self):
    # Variables
    fileName         = 'msys'    
    operatingSystem  = Globals.operating_system_
    libraries = [ "libcoinblas.a", "libcoinmetis.a", "libcoinmumps.a", "libcoinlapack.a", "libipopt.a" ]
    # Clean our any existing files if they already exist
    print '-- Cleaning files'  
    if os.path.exists(fileName):
      os.system('chmod 777 -R ' + fileName)
      shutil.rmtree(fileName)

    print '-- Removing previously built headers and libraries'
    os.system('rm -rf ' + Globals.target_include_path_ + "cppad")
    os.system('rm -rf ' + Globals.target_include_path_ + "coin")
    for library in libraries:
      os.system('rm -rf ' + Globals.target_special_lib_path_ + library)        
        
    # Decompress our archive
    print '-- Decompressing - check isam_unzip.log'
    if os.path.exists(fileName + '.zip'):
      os.system('unzip ' + fileName + '.zip 1> isam_unzip.log 2>&1')
    
    # Build
    os.chdir(fileName + '\\1.0\\')
    print '-- Starting msys to decompress and build the IPOPT + CPPAD libary'
    os.system("msys.bat --norxvt")

    count = 0
    while not os.path.exists('cppad/include/'):
      count += 1
      if count > 3600:
        print '-- Timeout exceeded while waiting for msys to compile IPOPT + CPPAD'
        return False
      if count % 120 == 0:
        print '-- Waiting for msys compilation to finish'
      time.sleep(1);

    time.sleep(5)
    
    # Move our headers and libraries
    print '-- Moving headers and libraries'
    dir_util.copy_tree('cppad/include/cppad', Globals.target_include_path_ + '/cppad/')
    dir_util.copy_tree('cppad/Ipopt-3.11.9/include/coin', Globals.target_include_path_ + '/coin/')
    for library in libraries:
      shutil.copy('cppad/Ipopt-3.11.9/lib/' + library, Globals.target_special_lib_path_)    
     
    return True
