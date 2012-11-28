import os
import os.path
import subprocess
import sys
import shutil
import fileinput
from distutils import dir_util

# Variables
admbFileName    = 'admb-11'
targetPath      = os.getenv('isam_third_party_target_directory')

# Clean our any existing files if they already exist
print '-- Cleaning ADMB files'
if os.path.exists(admbFileName):
    shutil.rmtree(admbFileName)

# Decompress our archive
print '-- Decompressing ADMB - check isam_unzip.log'
if os.path.exists(admbFileName + '.zip'):
    os.system('unzip ' + admbFileName + '.zip 1> isam_unzip.log 2>&1')

# Build ADMB
print '-- Building ADMB - check ' + admbFileName + '/isam_make.log for progress - estimated time 30-60 minutes'
os.chdir(admbFileName)
os.system("mingw32-make -j4 1> isam_make.log 2>&1")

if not os.path.exists('build/mingw/include') or not os.path.exists('build/mingw/lib'):
    print '## Failed to build ADMB - Check log file for error'
    sys.exit(-1) 
    
# Move our headers and libraries
print '-- Moving headers and libraries'
dir_util.copy_tree('build/mingw/include', targetPath + '/include/admb/')
dir_util.copy_tree('build/mingw/lib', targetPath + '/lib')
    
# Finally touch the success file to create it
os.system('touch ' + targetPath + '/ADMB.success')