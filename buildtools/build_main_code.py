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
buildType   = os.getenv('isam_build_type')
buildParam  = os.getenv('isam_build_param', '')

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
if buildParam != '':
    print '-> Build Parameter: ' + buildParam.upper()
    subprocess.call('cmake -G "' + cmakeCompiler + ' Makefiles" -D' + buildType.upper() + '=1 -D' +  buildParam.upper() + '=1 ..\\..\\..')
else:
    subprocess.call('cmake -G "' + cmakeCompiler + ' Makefiles" -D' + buildType.upper() + '=1 ..\\..\\..')

print '-> Building software. Build Type: ' + buildType.upper()
os.system('mingw32-make')
