import os
import sys
import os.path
import subprocess
import fileinput

# Get some system variables
operatingSystem = sys.platform
oldPath = os.getenv('PATH')
osPath = ''
cwd = os.getcwd() + '\\'

# Modify our system PATH environment variable based on OS
if operatingSystem == 'win32':        
    osPath += cwd + 'buildtools\\windows\\unixutils;'
    osPath += cwd + 'buildtools\\windows\\cmake\\bin;'   
    
newPath = osPath + oldPath
os.environ['PATH'] = newPath

# Find our Compiler and put it's location in the path
compiler = 'g++'
if operatingSystem == 'win32':
    compiler += '.exe'

os.system('which ' + compiler + ' > compiler.txt')

fi = fileinput.FileInput('compiler.txt')
compilerPath = ''
for line in fi:
    compilerPath = line

compilerPath = compilerPath.replace(compiler + '\n', '').rstrip()
osPath += compilerPath + ';'

os.system('rm -rf compiler.txt')

# Find cmd.exe and put it's locaiton in the path
if operatingSystem == 'win32':
    os.system('which cmd.exe > cmd.txt')
    
    fi = fileinput.FileInput('cmd.txt')
    cmdPath = ''
    for line in fi:
        cmdPath = line

    cmdPath = cmdPath.replace('cmd.exe\n', '').rstrip()
    osPath += cmdPath + ';'   

    os.system('rm -rf cmd.txt')    

# Change The Path
os.environ['PATH'] = osPath 
    
# Figure out what kind of build we need to do
buildType  = "DEBUG"
buildParam = ""
if len(sys.argv) > 1:
    buildType = sys.argv[1]
if len(sys.argv) > 2:
    buildParam = sys.argv[2]

if buildType.upper() == "HELP" or buildType == "?":
    os.system("cls")
    print '###########################################################'
    print '# iSAM Build System                                       #'
    print '###########################################################'
    print 'Usage:'
    print 'doBuild <buildType> <buildParam>'
    print ''
    print 'Valid Build Types:'
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
    
if buildType.upper() != "DEBUG" and buildType.upper() != "RELEASE" and buildType.upper() != "THIRDPARTY":
    sys.exit('Invalid build type ' + buildType + '. Supported build types are "release" and "debug" and "thirdparty"')    
    
os.putenv('isam_build_type', buildType)
os.putenv('isam_build_param', buildParam)    
    
# Start to build parts of the system
if buildType.upper() == "THIRDPARTY":
    subprocess.call('python ' + cwd + 'buildtools\\build_third_party.py')
else:
    subprocess.call('python ' + cwd + 'buildtools\\build_main_code.py ')