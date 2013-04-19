import os
import sys
import subprocess
import os.path

print ''
print '###########################################################################'
print 'Building Main Code Base'
print '###########################################################################'

EX_OK = getattr(os, "EX_OK", 0)

# Variables
operatingSystem = sys.platform
if operatingSystem == 'linux2' or operatingSystem == 'linux3':
 operatingSystem = 'linux'
print '--> Operating System: ' + operatingSystem

buildType   = os.getenv('isam_build_type')
buildParam  = os.getenv('isam_build_param', '')

if operatingSystem == 'win32':
    cmakeCompiler = 'MinGW'
else:
    cmakeCompiler = 'Unix'

# Change current working directory    
outputDirectory = 'build/' + operatingSystem  + '/' + buildType
if not os.path.exists(outputDirectory):  
    os.makedirs(outputDirectory)
    
os.chdir(outputDirectory)

print '-> Generating CMake File'
if buildParam != '':
    print '-> Build Parameter: ' + buildParam.upper()
    if os.system('cmake -G "' + cmakeCompiler + ' Makefiles" -D' + buildType.upper() + '=1 -D' +  buildParam.upper() + '=1 ../../..') != EX_OK:
        print '## ERROR: Failed to execute cmake successfully to rebuild the make files'
        sys.exit(1)
else:
    if os.system('cmake -G "' + cmakeCompiler + ' Makefiles" -D' + buildType.upper() + '=1 ../../..') != EX_OK:
        print '## ERROR: Failed to execute cmake successfully to rebuild the make files'

print '-> Building software. Build Type: ' + buildType.upper()
if operatingSystem == 'win32':
    if os.system('mingw32-make') != EX_OK:
        print 'Failed to build iSAM. Please see above for build error'
        sys.exit(1)
else:
    if os.system('make') != EX_OK:
        print 'Failed to build iSAM. Please see above for build error'
        sys.exit(1)
    
