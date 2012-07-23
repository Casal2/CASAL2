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
print 'osPath: ' + osPath
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
os.environ['PATH'] = osPath

os.system('rm -rf compiler.txt')
    
# Start to build parts of the system
subprocess.call('python ' + cwd + 'buildtools\\build_third_party.py')
