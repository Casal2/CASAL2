import os
import shutil
import time
from distutils import dir_util

import Globals

EX_OK = getattr(os, "EX_OK", 0)

class Builder:
  version_ = 3.11
    
  def start(self):
    # Variables
    ipOptFileName         = 'Ipopt-releases-3.12.4'
    cppadFileName         = 'CppAD-20160000.1'
    libraries = [ "libcoinblas.a", "libcoinmetis.a", "libcoinmumps.a", "libcoinlapack.a", "libipopt.a" ]
    
    # Clean our any existing files if they already exist
    print '-- Cleaning files'
    if os.path.exists(ipOptFileName):
      os.system('chmod 777 -R ' + ipOptFileName)
      shutil.rmtree(ipOptFileName)    
    if os.path.exists(cppadFileName):
      os.system('chmod 777 -R ' + cppadFileName)
      shutil.rmtree(cppadFileName)
    os.system('rm -rf *.tar')

    print '-- Removing previously built headers and libraries'
    os.system('rm -rf ' + Globals.target_include_path_ + "cppad")
    os.system('rm -rf ' + Globals.target_include_path_ + "coin")
    for library in libraries:
      os.system('rm -rf ' + Globals.target_special_lib_path_ + library)        
        
    # Decompress our archive
    print '-- Decompressing - check casal2_unzip.log'
    if os.path.exists(ipOptFileName + '.tar.gz'):
      os.system('tar xvzf ' + ipOptFileName + '.tar.gz 1> casal2_unzip.log 2>&1')
      # os.system('tar xvf ' + ipOptFileName + '.tar 1> casal2_untar.log 2>&1')
    if os.path.exists(cppadFileName + '.zip'):
      os.system('unzip ' + cppadFileName + '.zip 1> casal2_unzip.log 2>&1')

    # Build
    os.chdir(ipOptFileName)      
    if os.system("./configure --enable-static --disable-shared --with-pic 1> casal2_configure.log 2>&1") != EX_OK:
      return Globals.PrintError("Failed to configure code base. Please see above for build error")
    if os.system('make install 1> casal2_make_install.log 2>&1') != EX_OK:
      return Globals.PrintError("Failed to install code base. Please see above for build error")
    os.chdir('../')

    print '-- Building CppAD Library'
    os.chdir(cppadFileName)
    os.system('chmod +x configure')
    if os.system("./configure --prefix=$PWD 1> casal2_configure.log 2>&1") != EX_OK:
      return Globals.PrintError("Failed to configure code base. Please see above for build error")
    if os.system("make CXXFLAGS='-fPIC' 1> make.log 2>&1") != EX_OK:
      return Globals.PrintError("Failed to make code base. Please see above for build error")
    if os.system("make install 1> make_install.log 2>&1") != EX_OK:
      return Globals.PrintError("Failed to install code base. Please see above for build error")    
    os.chdir('../')
    
    # Move our headers and libraries
    print '-- Moving headers and libraries'
    dir_util.copy_tree(cppadFileName + '/include/cppad', Globals.target_include_path_ + '/cppad/')
    dir_util.copy_tree(ipOptFileName + '/include/coin', Globals.target_include_path_ + '/coin/')
    if os.path.exists(ipOptFileName + '/lib64'):
      for library in libraries:
        shutil.copy(ipOptFileName + '/lib64/' + library, Globals.target_special_lib_path_)    
    if os.path.exists(ipOptFileName + '/lib'):
      for library in libraries:
        shutil.copy(ipOptFileName + '/lib/' + library, Globals.target_special_lib_path_)
        
    return True
