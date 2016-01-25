import os
import sys
import subprocess
import os.path
import shutil
import fileinput
import re
from datetime import datetime, date
from dateutil import tz

EX_OK = getattr(os, "EX_OK", 0)

class DebBuilder:
  do_build_ = "doBuild"
  def start(self):
    if Globals.operating_system_ == "windows":
      self.do_build_ += '.bat'
    else:
      self.do_build_ = './' + self.do_build_ + '.sh'
    print '-- Starting Deb Builder'
    print '--> Building release version of CASAL2 binary'
    print '-- Re-Entering the build system to build a release binary'
    print '-- All output is being diverted to release_build.log'
    if os.system(self.do_build_ + ' release > release_build.log 2>&1') != EX_OK:
      return Globals.PrintError('Failed to build release binary. Please check release_build.log for the error')
    os.system('rm -rf release_build.log')

    print '--> Building documentation'
    print '-- Re-Entering the build system to build the documentation'
    print '-- All ourput is being diverted to documentation_build.log'
    if os.system(self.do_build_ + ' documentation > documentation_build.log 2>&1') != EX_OK:
      return Globals.PrintError('Failed to build the documentation. Please check documenation_build.log for error')
    os.system('rm -rf documentation_build.log')

    p = subprocess.Popen(['git', '--no-pager', 'log', '-n', '1', '--pretty=format:%H%n%h%n%ci' ], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()
    lines = out.split('\n')          
    if len(lines) != 3:
      return Globals.PrintError('Format printed by GIT did not meet expectations. Expected 3 lines but got ' + str(len(lines)))
    print '-- CASAL2 Revision: ' + lines[1]
    if not os.path.exists('bin/linux/deb'):
      os.mkdir('bin/linux/deb')

    folder = 'bin/linux/deb/CASAL2_' + lines[1]    
    os.system('rm -rf ' + folder)
    os.makedirs(folder + '/usr/local/bin')
    os.system('cp bin/linux/frontend/casal2 ' + folder + '/usr/local/bin')
    os.system('cp bin/linux/library_adolc/libcasal2.so ' + folder + '/usr/local/lib/casal2_adolc.so')
    os.system('cp bin/linux/library_betadiff/libcasal2.so ' + folder + '/usr/local/lib/casal2_betadiff.so')
    os.system('cp bin/linux/library_cppad/libcasal2.so ' + folder + '/usr/local/lib/casal2_cppad.so')
    os.system('cp bin/linux/library_release/libcasal2.so ' + folder + '/usr/local/lib/casal2_release.so')
    os.system('cp bin/linux/library_test/libcasal2.so ' + folder + '/usr/local/lib/casal2_test.so')
    os.makedirs(folder + '/opt/casal2')
    os.system('cp ../Documentation/Manual/CASAL2.pdf ' + folder + '/opt/casal2/')
    os.makedirs(folder + '/DEBIAN')
    control_file = open(folder + '/DEBIAN/control', 'w')
    control_file.write('Package: CASAL2\n')
    control_file.write('Version: 0x' + lines[1] + '\n')
    control_file.write('Section: base\n')
    control_file.write('Priority: optional\n')
    control_file.write('Architecture: amd64\n')
    control_file.write('Maintainer: Scott Rasmussen (Zaita) <scott@zaita.com>\n')
    control_file.write('Description: CASAL2 Modeling Platform\n')
    control_file.close()

    if os.system('dpkg-deb --build ' + folder) != EX_OK:
      return Globals.PrintError('Failed to build DEB Package')
    return True
