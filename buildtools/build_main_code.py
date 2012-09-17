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
buildType = os.getenv('isam_build_type')

if operatingSystem == 'win32':
    cmakeCompiler = 'MinGW'
else:
    cmakeCompiler = 'Unix'

# Change current working directory    
outputDirectory = 'build\\' + operatingSystem  + '\\' + buildType
if not os.path.exists(outputDirectory):  
    os.makedirs(outputDirectory)
    
os.chdir(outputDirectory)

print '-> Generating CMake File'
subprocess.call('cmake -G "' + cmakeCompiler + ' Makefiles" -D' + buildType.upper() +'=1 ..\\..\\..')

print '-> Building software. Build Type: ' + buildType.upper()
subprocess.call('make')
