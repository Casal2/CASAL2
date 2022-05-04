


import os
import sys
import subprocess
import os.path
import shutil
import fileinput
import re
from datetime import datetime, date
import Globals
from Builder import *

EX_OK = getattr(os, "EX_OK", 0)

class DebBuilder:
  do_build_ = "doBuild"
  def start(self, skip_building = 'false'):
    print('-- Starting Deb Builder')
    print('-- Skip Building Archive? ' + skip_building)

    if skip_building != 'false':
      if Globals.operating_system_ == "windows":
        self.do_build_ += '.bat'
      else:
        self.do_build_ = './' + self.do_build_ + '.sh'
      print('--> Building Casal2 Archive')
      print('-- Re-Entering the build system to build a release binary')
      if subprocess.call(self.do_build_ + ' archive', shell=True) != EX_OK:
        return Globals.PrintError('Failed to build Casal2 archive')

    p = subprocess.Popen(['git', '--no-pager', 'log', '-n', '1', '--pretty=format:%H%n%h%n%ci' ], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    out, err = p.communicate()
    lines = out.split('\n')
    if len(lines) != 3:
      return Globals.PrintError('Format printed by git did not meet expectations. Expected 3 lines but got ' + str(len(lines)))
    print('-- Casal2 Revision: ' + lines[1])
    if not os.path.exists('bin/linux/deb'):
      os.mkdir('bin/linux/deb')
    print(lines[1])
    folder = 'bin/linux/deb/Casal2'
    subprocess.call('rm -rf ' + folder, shell=True)
    os.makedirs(folder + '/usr/local/bin')
    os.makedirs(folder + '/usr/local/lib')
    subprocess.call('cp Casal2/casal2 ' + folder + '/usr/local/bin', shell=True)
    subprocess.call('cp Casal2/casal2_adolc.so ' + folder + '/usr/local/bin', shell=True)
    subprocess.call('cp Casal2/casal2_betadiff.so ' + folder + '/usr/local/bin', shell=True)
    subprocess.call('cp Casal2/casal2_release.so ' + folder + '/usr/local/bin', shell=True)
    subprocess.call('cp Casal2/casal2_test.so ' + folder + '/usr/local/bin', shell=True)
    os.makedirs(folder + '/usr/local/share/doc/casal2')
    os.makedirs(folder + '/usr/local/share/doc/ContributorsGuide')
    os.makedirs(folder + '/usr/local/share/doc/GettingStartedGuide')
    os.makedirs(folder + '/usr/local/share/doc/README')
    os.makedirs(folder + '/usr/local/share/doc/R-library/')
    os.makedirs(folder + '/usr/local/share/doc/Examples')
    subprocess.call('cp ../Documentation/UserManual/CASAL2_Age.pdf ' + folder + '/usr/local/share/doc/casal2/', shell=True)
    subprocess.call('cp ../Documentation/UserManual/CASAL2_Length.pdf ' + folder + '/usr/local/share/doc/casal2/', shell=True)
    subprocess.call('cp ../Documentation/ContributorsManual/ContributorsGuide.pdf ' + folder + '/usr/local/share/doc/ContributorsGuide/', shell=True)
    subprocess.call('cp ../Documentation/GettingStartedGuide/GettingStartedGuide.pdf ' + folder + '/usr/local/share/doc/GettingStartedGuide/', shell=True)
    subprocess.call('cp ../README.txt ' + folder + '/usr/local/share/doc/README/', shell=True)
    subprocess.call('cp -a ../Examples/. ' + folder + '/usr/local/share/doc/Examples/', shell=True)
    subprocess.call('cp ../"R-libraries"/Casal2_' + Globals.Casal2_version_number + '.tar.gz ' + folder + '/usr/local/share/doc/R-library/Casal2_' + Globals.Casal2_version_number + '.tar.gz', shell=True)
    os.makedirs(folder + '/DEBIAN')
    control_file = open(folder + '/DEBIAN/control', 'w')
    control_file.write('Package: Casal2\n')
    control_file.write('Version: 0x' + lines[1] + '\n')
    control_file.write('Section: base\n')
    control_file.write('Priority: optional\n')
    control_file.write('Architecture: amd64\n')
    control_file.write('Maintainer: NIWA <casal2@niwa.co.nz>\n')
    control_file.write('Description: Casal2 Modelling Platform\n')
    control_file.close()

    if subprocess.call('dpkg-deb --build ' + folder, shell=True) != EX_OK:
      return Globals.PrintError('Failed to build DEB Package')
    return True
