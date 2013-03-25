import os
import sys
import os.path
import subprocess
import fileinput

# Get some system variables
operatingSystem = sys.platform
oldPath = os.getenv('PATH')
osPath = ''
cwd = os.path.normpath(os.getcwd())

if operatingSystem == 'linux2' or operatingSystem == 'linux3':
 operatingSystem = 'linux'

# Modify our system PATH environment variable based on OS
if operatingSystem == 'win32':        
    osPath += cwd + '\\buildtools\\windows\\unixutils;'
    osPath += cwd + '\\buildtools\\windows\\cmake\\bin;'   
    osPath += cwd + '\\buildtools\\windows\\Python27\\;'   
    
newPath = osPath + oldPath
os.environ['PATH'] = newPath

# Find our Compiler and put it's location in the path
compiler = 'g++'
if operatingSystem == 'win32':
    compiler += '.exe'

if os.system('which ' + compiler + ' > compiler.txt') != os.EX_OK:
    print '## ERROR: Failed to execute \'which\' command to find the currently installed compiler'
    sys.exit(1)

fi = fileinput.FileInput('compiler.txt')
compilerPath = ''
for line in fi:
    compilerPath = line

compilerPath = compilerPath.replace(compiler + '\n', '').rstrip()
osPath += compilerPath + ';'

if os.system('rm -rf compiler.txt') != os.EX_OK:
    print '## ERROR: Failed to remove the compiler.txt after reading the compiler path'
    sys.exit(1)

# Find cmd.exe and put it's locaiton in the path
if operatingSystem == 'win32':
    if os.system('which cmd.exe > cmd.txt') != os.EX_OK:
        print '## ERROR: Failed to execute \'which\' command to find the currently installed cmd.exe path'
        sys.exit(1)
    
    fi = fileinput.FileInput('cmd.txt')
    cmdPath = ''
    for line in fi:
        cmdPath = line

    cmdPath = cmdPath.replace('cmd.exe\n', '').rstrip()
    osPath += cmdPath + ';'   

    if os.system('rm -rf cmd.txt') != os.EX_OK:
        print '## ERROR: Failed to remove the cmd.txt after reading the cmd.exe path'
        sys.exit(1)

# Change The Path
if operatingSystem != 'win32':
    osPath += newPath
os.environ['PATH'] = osPath 
    
# Figure out what kind of build we need to do
buildType  = "DEBUG"
buildParam = ""

if len(sys.argv) > 1 and len(str(sys.argv[1])) > 1:
    buildType = sys.argv[1]
if len(sys.argv) > 2 and len(str(sys.argv[2])) > 1:
    buildParam = sys.argv[2]

if buildType.upper() == "HELP" or buildType == "?":
    print '###########################################################'
    print '# iSAM Build System                                       #'
    print '###########################################################'
    print 'Usage:'
    print 'doBuild <buildType> <buildParam>'
    print ''
    print 'Valid Build Types:'
    print '  clean - Remove any previous debug/release build information'
    print '  cleanall - Remove all previous build information'
    print '  thirdparty - Builds required third party libraries'
    print '  debug - Builds a debug version of iSAM (default)'
    print '  release - Builds a release version of iSAM'
    print ''
    print 'Valid Build Parameters: (thirdparty only)'
    print '  <libary name> - Target third party library to build or rebuild'
    print ''
    print 'Valid Build parameters: (debug/release only)'
    print '  admb - Use ADMB auto-differentiation in compiled executable'
    sys.exit()    
    
if buildType.upper() != "DEBUG" and buildType.upper() != "RELEASE" and buildType.upper() != "THIRDPARTY" and buildType.upper() != "CLEAN" and buildType.upper() != "CLEANALL":
    print 'Invalid build type ' + buildType + '. Supported build types are "release", "debug", "clean", "cleanall" and "thirdparty"'
    sys.exit()    
    
os.putenv('isam_build_type', buildType.lower())
os.putenv('isam_build_param', buildParam.lower())    
os.putenv('isam_operating_system', operatingSystem.lower())
    
# Start to build parts of the system
if buildType.upper() == "THIRDPARTY":
    if os.system('python ' + cwd + '/buildtools/build_third_party.py') != os.EX_OK:
        sys.exit(1)
elif buildType.upper() == "CLEAN":
    print '-> Cleaning previous build information'
    os.system('rm -rf build/win32/debug')    
    os.system('rm -rf build/win32/release')    
    os.system('rm -rf build/linux/debug')    
    os.system('rm -rf build/linux/release')  
elif buildType.upper() == "CLEANALL":    
    print '-> Cleaning all previous build information'
    os.system('rm -rf build/win32/debug')    
    os.system('rm -rf build/win32/release')  
    os.system('rm -rf build/win32/thirdparty')
    os.system('rm -rf build/linux/debug')    
    os.system('rm -rf build/linux/release')  
    os.system('rm -rf build/linux/thirdparty')
else:
    command = 'python ' + os.path.normpath(cwd + '/buildtools/build_main_code.py')
    if os.system(command) != os.EX_OK:
        sys.exit(1)
    
sys.exit()