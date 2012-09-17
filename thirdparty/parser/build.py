import os
import os.path
import subprocess
import sys
import shutil

# Variables
fileName = 'parser'
targetPath    = os.getenv('isam_third_party_target_directory')

# Clean our any existing files if they already exist
print '-- Cleaning Parser files'
if os.path.exists(fileName):
    shutil.rmtree(fileName)

# Decompress our archive
print '-- Decompressing Parser - check isam_unzip.log'
if os.path.exists(fileName + '.zip'):
    os.system('unzip ' + fileName + '.zip 1> isam_unzip.log 2>&1')

# Build 
print '-- Building Parser - check parser\\isam_build.log'
os.chdir(fileName)
os.system("g++ -g0 -c parser.cpp 1> isam_build.log 2>&1")
os.system("ar rvs parser.a parser.o 1>> isam_build.log 2>>&1")
    
# Move our headers and libraries
print '-- Moving headers and libraries'
destPath = '..\\..\\..\\' + targetPath + '\\'

os.makedirs(destPath + "\\include")
os.makedirs(destPath + "\\lib")

shutil.move('parser.a', destPath + "\\lib\\")
shutil.move('parser.h', destPath + "\\include\\")

# Finally touch the success file to create it
os.system('touch ..\\..\\..\\' + targetPath + '\\isam.success')