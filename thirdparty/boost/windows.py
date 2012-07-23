import os
import os.path

# Variables
boostFileName = 'boost_1_50_0'
targetPath    = os.getenv('isam_third_party_target_directory')

# Clean our any existing files if they already exist
print 'Cleaning Boost files'
if os.path.exists('boostFileName'):
    os.system('rm -rf ' + boostFileName)

# Make a copy of boost.tar.gz as temp.tar.gz if one doesn't exist
if not os.path.exists('temp.tar') and not os.path.exists('temp.tar.gz'):
    print 'Copying Boost archive'
    os.system('cp ' + boostFileName + '.tar.gz temp.tar.gz')

# Decompress our boost archive
print 'Decompressing Boost'
if os.path.exists('temp.tar.gz'):
    os.system('gzip -d temp.tar.gz')

if os.path.exists('temp.tar'):
    os.system('tar -x -ftemp.tar')
    os.system('rm -rf temp.tar')

# Build Boost
print 'Building Boost - check isam_*.log for progress'
oldPath = os.getcwd()
os.chdir(boostFileName)

os.system('bootstrap.bat gcc > isam_bootstrap.log')
os.system('b2.exe --toolset=gcc runtime-link=static threading=multi > isam_build.log')
    
# Move our headers and libraries
print 'Moving headers and libraries'
os.system('mv boost ..\\..\\..\\' + targetPath + '\\headers')
os.system('mv stage\\lib ..\\..\\..\\' + targetPath + '\\lib')

os.chdir(oldPath)