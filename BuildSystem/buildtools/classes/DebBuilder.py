import os
import sys
import subprocess
import os.path
import shutil
import fileinput
import re
from datetime import datetime, date
from dateutil import tz
import Globals
from Builder import *

EX_OK = getattr(os, "EX_OK", 0)

class DebBuilder:
  do_build_ = "doBuild"
  def start(self, skip_building = 'false'):
    print '-- Starting Deb Builder'
    print '-- Skip Building Archive? ' + skip_building

    if skip_building != 'false':
      if Globals.operating_system_ == "windows":
        self.do_build_ += '.bat'
      else:
        self.do_build_ = './' + self.do_build_ + '.sh'
      print '--> Building CASAL2 Archive'
      print '-- Re-Entering the build sytem to build a release binary'
      if os.system(self.do_build_ + ' archive') != EX_OK:
        return Globals.PrintError('Failed to build CASAL2 archive')

    p = subprocess.Popen(['git', '--no-pager', 'log', '-n', '1', '--pretty=format:%H%n%h%n%ci' ], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()
    lines = out.split('\n')          
    if len(lines) != 3:
      return Globals.PrintError('Format printed by GIT did not meet expectations. Expected 3 lines but got ' + str(len(lines)))
    print '-- CASAL2 Revision: ' + lines[1]
    if not os.path.exists('bin/linux/deb'):
      os.mkdir('bin/linux/deb')

    folder = 'bin/linux/deb/Casal2_' + lines[1]    
    os.system('rm -rf ' + folder)
    os.makedirs(folder + '/usr/local/bin')
    os.makedirs(folder + '/usr/local/lib')
    os.system('cp CASAL2/casal2 ' + folder + '/usr/local/bin')
    os.system('cp CASAL2/casal2_adolc.so ' + folder + '/usr/local/lib')
    os.system('cp CASAL2/casal2_betadiff.so ' + folder + '/usr/local/lib')
    os.system('cp CASAL2/casal2_cppad.so ' + folder + '/usr/local/lib')
    os.system('cp CASAL2/casal2_release.so ' + folder + '/usr/local/lib')
    os.system('cp CASAL2/casal2_test.so ' + folder + '/usr/local/lib')
    os.makedirs(folder + '/usr/local/share/doc/casal2')
    os.system('cp ../Documentation/Manual/CASAL2.pdf ' + folder + '/usr/local/share/doc/casal2/')
    os.makedirs(folder + '/DEBIAN')
    control_file = open(folder + '/DEBIAN/control', 'w')
    control_file.write('Package: Casal2\n')
    control_file.write('Version: 0x' + lines[1] + '\n')
    control_file.write('Section: base\n')
    control_file.write('Priority: optional\n')
    control_file.write('Architecture: amd64\n')
    control_file.write('Maintainer: Scott Rasmussen (Zaita) <scott@zaita.com>\n')
    control_file.write('Description: Casal2 Modeling Platform\n')
    control_file.close()

    if os.system('dpkg-deb --build ' + folder) != EX_OK:
      return Globals.PrintError('Failed to build DEB Package')
    return True
