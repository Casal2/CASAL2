import os
import sys
import subprocess
import os.path
import fileinput
import re
from datetime import datetime, date
from dateutil import tz

import Globals

EX_OK = getattr(os, "EX_OK", 0)

class Installer:
  def start(self):
    file = open('config.iss', 'w')
    if not file:
      return Globals.PrintError('Failed to open the config.iss to create installer configuration')
    file.write('[Setup]\n')
    file.write('AppName=CASAL2\n')

    # get the version information
    print '-- Loading version information from GIT'
    p = subprocess.Popen(['git', '--no-pager', 'log', '-n', '1', '--pretty=format:%H%n%h%n%ci' ], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()
    lines = out.split('\n')          
    if len(lines) != 3:
      return Globals.PrintError('Format printed by GIT did not meet expectations. Expected 3 lines but got ' + str(len(lines)))

    time_pieces = lines[2].split(' ')
    del time_pieces[-1];
    temp = ' '.join(time_pieces)
    local_time = datetime.strptime(temp, '%Y-%m-%d %H:%M:%S')
    utc_time   = local_time.replace(tzinfo=tz.tzlocal()).astimezone(tz.tzutc())

    version = utc_time.strftime('%Y%m%d') + '.' + lines[1]
    file.write('AppVersion=' + version + '\n')
    file.write('AppPublisher=NIWA\n')
    file.write('AppPublisherURL=http://www.niwa.co.nz\n')
    file.write('AppSupportURL=http://www.niwa.co.nz\n')
    file.write('AppUpdatesURL=http://www.niwa.co.nz\n')
    file.write('DefaultDirName=C:\\CASAL2\n')
    file.write('DefaultGroupName=CASAL2\n')
    file.write('AllowNoIcons=Yes\n')
    file.write('OutputBaseFileName=casal2_setup\n')
    file.write('Compression=lzma\n')
    file.write('SolidCompression=yes\n')
    file.write('[Tasks]\n')
    file.write('Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}";\n')
    file.write('Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}";\n')
    file.write('[Files]\n')
    file.write('Source: "bin\\windows\\release\\casal2.exe"; DestDir: "{app}"; Flags: ignoreversion\n')
    file.write('Source: "..\\Documentation\\Manual\\CASAL2.pdf"; DestDir: "{app}"; Flags: ignoreversion\n')
    file.write('[Icons]\n')
    file.write('Name: "{group}\\CASAL2"; Filename: "{app}"; WorkingDir: "{app}";\n')
    file.write('Name: "{userdesktop}\\CASAL2"; Filename: "{app}"; WorkingDir: "{app}"; Tasks: desktopicon\n')
    file.close()
 
    os.system('"C:\\Program Files (x86)\\Inno Setup 5\\ISCC.exe" config.iss')

