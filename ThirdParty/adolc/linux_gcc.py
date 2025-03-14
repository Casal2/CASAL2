import os
import shutil

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
    print('-- Cleaning files'  )
    if os.path.exists(fileName):
      os.system('chmod 777 -R ' + fileName)
      shutil.rmtree(fileName)

    print('-- Removing previously built headers and libraries')
    os.system('rm -rf ' + Globals.target_include_path_ + headers)
    os.system('rm -rf ' + Globals.target_special_lib_path_ + library)

    # Decompress our archive
    print('-- Decompressing - check casal2_unzip.log')
    if os.path.exists(fileName + '.zip'):
      os.system('unzip ' + fileName + '.zip 1> casal2_unzip.log 2>&1')

    # Build
    os.chdir(fileName)
    print('-- Configuring - check casal2_configure.log')
    os.system("./configure --disable-shared --enable-static --with-pic 1> casal2_configure.log 2>&1")
    print('-- Building - check casal2_build.log')
    os.system("make 1> casal2_build.log 2>&1")

    # Move our headers and libraries
    print('-- Moving headers and libraries')
    os.chdir('ADOL-C/')
    shutil.copytree('include/' + headers, Globals.target_include_path_ + '/adolc/', dirs_exist_ok=True)
    shutil.copy('.libs/' + library, Globals.target_special_lib_path_)

    return True

