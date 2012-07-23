import os
import sys
import subprocess
import os.path

print ''
print '###########################################################################'
print 'Building Third-Party Libraries'
print '###########################################################################'

# Variables
path="thirdparty\\"
operatingSystem = sys.platform

# Ensure Destination Path is ready
thirdPartyBuildFolder = 'build\\' + operatingSystem + '\\thirdparty'
if not os.path.exists(thirdPartyBuildFolder):
    os.makedirs(thirdPartyBuildFolder);   

# Inside every thirdparty folder will exist either
# build.py - Build file for all platforms
# windows.py - Build file for Windows
# linux.py - Build file for Linux
dirList=os.listdir(path)
for fname in dirList:    
    # Setup the target directories for our output
    targetDirectory = 'build\\' + operatingSystem + '\\thirdparty\\' + fname
    if os.path.exists(targetDirectory):    
        print 'Skipping library: ' + fname 
        continue
    else:
        os.mkdir(targetDirectory)
    
    print ''
    print '--> Building Library: ' + fname
    
    # Build Environment Variables
    os.putenv('isam_third_party_target_directory', targetDirectory)
    os.putenv('isam_operating_system', operatingSystem)
    
    # Change current working directory        
    os.chdir(path + fname + "\\")
    
    # Call appropriate build script
    if os.path.exists('build.py'):
        subprocess.call('python build.py')
    else:
        if operatingSystem == 'win32':
            subprocess.call('python windows.py')
        elif operatingSystem == 'linux':
            subprocess.call('python linux.py')
