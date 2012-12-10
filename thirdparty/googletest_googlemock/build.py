import os
import os.path
import subprocess
import sys
import shutil
from distutils import dir_util

# Variables
fileName = 'gmock-1.6.0'
targetPath    = os.getenv('isam_third_party_target_directory')

# Clean our any existing files if they already exist
print '-- Cleaning files'
if os.path.exists(fileName):
    shutil.rmtree(fileName)

# Decompress our archive
print '-- Decompressing - check isam_unzip.log'
if os.path.exists(fileName + '.zip'):
    os.system('unzip ' + fileName + '.zip 1> isam_unzip.log 2>&1')

# Build 
print '-- Building - check ' + fileName +'/make/isam_build.log'
os.chdir(fileName + '/make')
os.system("make 1> isam_build.log 2>&1")
    
# Move our headers and libraries
print '-- Moving headers and libraries'
shutil.copy('gmock_main.a', targetPath + "/lib/")
dir_util.copy_tree('../include/', targetPath + '/include/')
dir_util.copy_tree('../gtest/include/', targetPath + '/include/')

# Finally touch the success file to create it
os.system('touch ' + targetPath + '/googletest_googlemock.success')