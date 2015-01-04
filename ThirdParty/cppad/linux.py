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
    ipOptFileName         = 'Ipopt-3.11.9'
    cppadFileName         = 'cppad-20141128'
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
    print '-- Decompressing - check isam_unzip.log'
    if os.path.exists(ipOptFileName + '.tgz'):
      os.system('gzip -dk ' + ipOptFileName + '.tgz 1> isam_unzip.log 2>&1')
      os.system('tar xvf ' + ipOptFileName + '.tar 1> isam_untar.log 2>&1')
    if os.path.exists(cppadFileName + '.zip'):
      os.system('unzip ' + cppadFileName + '.zip 1> isam_unzip.log 2>&1')
    
    # Build
    os.chdir(ipOptFileName + '/ThirdParty/ASL')      
    print '-- Building IpOPT Library'
    if os.system('./get.ASL 1> isam_get.log 2>&1') != EX_OK:
      return Globals.PrintError("Failed to download third party library. Please see isam_get.log for error")
    os.chdir('../Blas')
    if os.system('./get.Blas 1> isam_get.log 2>&1') != EX_OK:
      return Globals.PrintError("Failed to download third party library. Please see isam_get.log for error")
    os.chdir('../Lapack')
    if os.system('./get.Lapack 1> isam_get.log 2>&1') != EX_OK:
      return Globals.PrintError("Failed to download third party library. Please see isam_get.log for error")
    os.chdir('../Metis')
    if os.system('./get.Metis 1> isam_get.log 2>&1') != EX_OK:
      return Globals.PrintError("Failed to download third party library. Please see isam_get.log for error")
    os.chdir('../Mumps')
    if os.system('./get.Mumps 1> isam_get.log 2>&1') != EX_OK:
      return Globals.PrintError("Failed to download third party library. Please see isam_get.log for error")
    os.chdir('../../')
    if os.system('./configure --enable-static --disable-shared 1> isam_configure.log 2>&1') != EX_OK:
      return Globals.PrintError("Failed to configure code base. Please see above for build error")
    if os.system('make 1> isam_make.log 2>&1') != EX_OK:
      return Globals.PrintError("Failed to build code base. Please see above for build error")
    if os.system('make install 1> isam_make_install.log 2>&1') != EX_OK:
      return Globals.PrintError("Failed to install code base. Please see above for build error")
    os.chdir('../')

    print '-- Building CppAD Library'
    os.chdir(cppadFileName)
    if os.system('./configure --prefix=$PWD 1> isam_configure.log 2>&1') != EX_OK:
      return Globals.PrintError("Failed to configure code base. Please see above for build error")                      
    if os.system('make install 1> make_install.log 2>&1') != EX_OK:
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
