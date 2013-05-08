import os
import os.path
import shutil
from distutils import dir_util

import Globals

def doBuild():
    # Variables
    boostFileName = 'boost_1_53_0'
    targetPath    = Globals.target_path_

    # Clean our any existing files if they already exist
    print '-- Cleaning Boost files'
    if os.path.exists(boostFileName):
        shutil.rmtree(boostFileName)

    # Make a copy of boost.tar.gz as temp.tar.gz if one doesn't exist
    if not os.path.exists('temp.tar') and not os.path.exists('temp.tar.gz'):
        print '-- Copying Boost archive'
        os.system('cp ' + boostFileName + '.tar.gz temp.tar.gz')

    # Decompress our boost archive
    print '-- Decompressing Boost'
    if os.path.exists('temp.tar.gz'):
        os.system('gzip -d temp.tar.gz')

    if os.path.exists('temp.tar'):
        os.system('tar -x -ftemp.tar')
        os.system('rm -rf temp.tar')

    # Build Boost
    print '-- Building Boost - check isam_build.log for progress - estimated time 30 minutes'
    os.chdir(boostFileName)

    os.system('./bootstrap.sh gcc 1> isam_bootstrap.log 2>&1')
    os.system('./b2 --toolset=gcc link=static runtime-link=static threading=multi 1> isam_build.log 2>&1')
        
    # Move our headers and libraries
    print '-- Moving headers and libraries'
    dir_util.copy_tree('boost', targetPath + '/include/boost/')
    shutil.copy('stage/lib/libboost_program_options.a', targetPath + "/lib/")

    os.system('touch ' + targetPath + '/boost.success')
    return True