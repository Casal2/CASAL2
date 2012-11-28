import os
import sys
import subprocess
import os.path

print ''
print '###########################################################################'
print 'Building Main Code Base'
print '###########################################################################'

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
    os.system('cmake -G "' + cmakeCompiler + ' Makefiles" -D' + buildType.upper() + '=1 -D' +  buildParam.upper() + '=1 ../../..')
else:
    os.system('cmake -G "' + cmakeCompiler + ' Makefiles" -D' + buildType.upper() + '=1 ../../..')

print '-> Building software. Build Type: ' + buildType.upper()
if operatingSystem == 'win32':
    os.system('mingw32-make')
else:
    os.system('make')

