import os
import sys
import subprocess
import os.path
import shutil

print ''
print '###########################################################################'
print 'Building Third-Party Libraries'
print '###########################################################################'

# Variables
path="thirdparty\\"
operatingSystem = sys.platform
cwd = os.getcwd()
buildParam = os.getenv('isam_build_param', '')   

# Ensure Destination Paths is ready
thirdPartyBuildFolder = 'build\\' + operatingSystem + '\\thirdparty'
if not os.path.exists(thirdPartyBuildFolder):
    os.makedirs(thirdPartyBuildFolder)
    
thirdPartyIncludeFolder = 'build\\' + operatingSystem + '\\thirdparty\\include'
if not os.path.exists(thirdPartyIncludeFolder):
    os.makedirs(thirdPartyIncludeFolder)

thirdPartyLibFolder = 'build\\' + operatingSystem + '\\thirdparty\\lib'
if not os.path.exists(thirdPartyLibFolder):
    os.makedirs(thirdPartyLibFolder)    
        
# Inside every thirdparty folder will exist either
# build.py - Build file for all platforms
# windows.py - Build file for Windows
# linux.py - Build file for Linux
dirList=os.listdir(path)
for fname in dirList: 
    # Allow us to force the build of a third-party library by specifying it's name on the command line
    if buildParam != "" and buildParam.upper() != fname.upper():
        continue
        
    os.chdir(cwd)
    
    # Setup the target directories for our output
    targetDirectory = 'build\\' + operatingSystem + '\\thirdparty\\'
    if os.path.exists(targetDirectory + '\\' + fname + '.success') and buildParam == "":   
        print '--> Skipping library: ' + fname + ' (already built) '
        continue
   
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

    os.chdir(cwd)
    if not os.path.exists(targetDirectory + "\\" + fname + ".success"):                
        #shutil.rmtree(targetDirectory, True)
        print '## ERROR - Failed to build third party library: ' + fname
        sys.exit(-1)               

                
                
                
                
                
                
                
                
                
                