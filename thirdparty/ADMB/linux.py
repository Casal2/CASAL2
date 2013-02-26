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
operatingSystem = os.getenv('isam_operating_system')

# Clean our any existing files if they already exist
print '-- Cleaning ADMB files'
if os.path.exists(admbFileName):
    shutil.rmtree(admbFileName)

# Decompress our archive
print '-- Decompressing ADMB - check isam_unzip.log'
if os.path.exists(admbFileName + '.zip'):
    os.system('unzip ' + admbFileName + '.zip 1> isam_unzip.log 2>&1')

# Build Configure scripts
print '-- Configuring ADMB - check ' + admbFileName + '/isam_configure.log for progress'
os.chdir(admbFileName)
os.system("make --directory=scripts/configure/ 1> isam_configure.log 2>&1")
os.system("chmod +x configure")
os.system("./configure 1>> isam_configure.log 2>&1")
    
# Build ADMB
print '-- Building ADMB - check ' + admbFileName + '/isam_make.log for progress - estimated time up to 30 minutes'
os.system("make -j4 1>> isam_make.log 2>&1")

# Figure out where the binaries have been built too
os.system('ls -1 build/ | grep "admb" > isam_build_folder.txt')
fi = fileinput.FileInput('isam_build_folder.txt')
buildFolder = ''
for line in fi:
    buildFolder = line.replace('\n', '').rstrip()

if not os.path.exists('build/' + buildFolder + '/include'):
    print '## Failed to build ADMB - Check log file for error'
    print '## include folder is not present in the build directory'
    sys.exit(-1) 

if not os.path.exists('build/' + buildFolder + '/lib'):
    print '## Failed to build ADMB - Check log file for error'
    print '## lib folder is not present in the build directory'
    sys.exit(-1) 
    
# Move our headers and libraries
print '-- Moving headers and libraries'
dir_util.copy_tree('build/' + buildFolder + '/include', targetPath + '/include/admb/')
dir_util.copy_tree('build/' + buildFolder + '/lib', targetPath + '/lib')
    
# Finally touch the success file to create it
os.system('touch ' + targetPath + '/ADMB.success')
