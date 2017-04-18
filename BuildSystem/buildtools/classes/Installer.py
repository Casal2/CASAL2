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
  do_build_ = "doBuild"
  def start(self):
    if Globals.operating_system_ == "windows":
      self.do_build_ += '.bat'
    else:
      self.do_build_ = './' + self.do_build_ + '.sh'

    print '--> Building CASAL2 Archive'
    print '-- Re-Entering build system to build the archive'
    print '-- Expected build time 10-60 minutes'
    if os.system(self.do_build_ + ' archive') != EX_OK:
      return Globals.PrintError('Failed to build the archive')

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
    file.write('AppId = CASAL2_001\n')    
    file.write('AppPublisher=NIWA\n')
    file.write('AppPublisherURL=http://www.niwa.co.nz\n')
    file.write('AppSupportURL=http://www.niwa.co.nz\n')
    file.write('AppUpdatesURL=http://www.niwa.co.nz\n')
    file.write('ChangesAssociations=yes\n')
    file.write('ChangesEnvironment=yes\n')
    file.write('DirExistsWarning= auto\n')        
    file.write('DisableDirPage = no\n')
    file.write('DefaultDirName={pf}\\CASAL2\\\n')
    file.write('DefaultGroupName=CASAL2\n')
    file.write('AllowNoIcons=Yes\n')
    file.write('SetupIconFile=casal2.ico\n')
    file.write('OutputBaseFileName=casal2_setup\n')
    file.write('Compression=lzma\n')
    file.write('SolidCompression=yes\n')
    file.write('[CustomMessages]\n')
    file.write('WizardImageFile=casal2.bmp\n')
    file.write('WizardSmallImageFile=casal2.bmp\n')
    file.write('[Tasks]\n')
    file.write('Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}";\n')
    file.write('Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}";\n')
    file.write('[Files]\n')
    file.write('Source: "casal2.ico"; DestDir: "{app}"; Flags: ignoreversion\n')
    file.write('Source: "run Casal2.lnk"; DestDir: "{app}"; Flags: ignoreversion\n')  
    file.write('Source: "Casal2\\casal2.exe"; DestDir: "{app}"; Flags: ignoreversion\n')
    file.write('Source: "Casal2\\casal2_adolc.dll"; DestDir: "{app}"; Flags: ignoreversion\n')
    file.write('Source: "Casal2\\casal2_betadiff.dll"; DestDir: "{app}"; Flags: ignoreversion\n')
    file.write('Source: "Casal2\\casal2_cppad.dll"; DestDir: "{app}"; Flags: ignoreversion\n')
    file.write('Source: "Casal2\\casal2_release.dll"; DestDir: "{app}"; Flags: ignoreversion\n')
    file.write('Source: "Casal2\\casal2_test.dll"; DestDir: "{app}"; Flags: ignoreversion\n')
    file.write('Source: "Casal2\\UserManual.pdf"; DestDir: "{app}"; Flags: ignoreversion\n')
    file.write('Source: "Casal2\\R-Libraries\\*"; DestDir: "{app}\R-Libraries"; Flags: replacesameversion recursesubdirs\n')
    file.write('Source: "Casal2\\Examples\\*"; DestDir: "{app}\Examples"; Flags: replacesameversion recursesubdirs\n')
    file.write('Source: "Casal2\\README.txt"; DestDir: "{app}"; Flags: ignoreversion\n')
    file.write('Source: "Casal2\\GettingStartedGuide.pdf"; DestDir: "{app}"; Flags: ignoreversion\n')
    file.write('Source: "Casal2\\ContributorsGuide.pdf"; DestDir: "{app}"; Flags: ignoreversion\n') 
    file.write('Source: "Casal2\\CASAL2.syn"; DestDir: "{app}"; Flags: ignoreversion\n') 
    file.write('Source: "Casal2\\TextPad_syntax_highlighter.readme"; DestDir: "{app}"; Flags: ignoreversion\n')            
    file.write('[Registry]\n')
    file.write('Root: HKLM; Subkey: "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment"; ValueType: expandsz; ValueName: "Path"; ValueData: "{olddata};{app}"; \\\n')
    file.write('     Check: NeedsAddPath(\'{app}\')\n')
    file.write('Root: HKCR; Subkey: ".csl2"; ValueType: string; ValueName: ""; ValueData: "Casal2ConfigFile"; Flags: uninsdeletevalue\n')
    file.write('Root: HKCR; Subkey: "Casal2ConfigFile"; ValueType: string; ValueName: ""; ValueData: "Casal2 input configuration file"; Flags: uninsdeletekey\n')
    file.write('Root: HKCR; Subkey: "Casal2ConfigFile\\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\\casal2.exe,0"\n')
    file.write('Root: HKCR; Subkey: "Casal2ConfigFile\\shell\\open\\command"; ValueType: string; ValueName: ""; ValueData: """{app}\\casal2.exe"" ""%1"""\n')
    file.write('[Icons]\n')
    file.write('Name: "{group}\\run casal2"; Filename: "{app}\\run Casal2.lnk"; WorkingDir: {app}; Tasks:quicklaunchicon\n')
    file.write('Name: "{group}\\Readme"; Filename: "{app}\README.txt"; WorkingDir: {app}\n')
    file.write('Name: "{group}\\CASAL2 user manual"; Filename: "{app}\UserManual.pdf"; WorkingDir: {app}\n')
    file.write('Name: "{group}\\Getting started guide"; Filename: "{app}\GettingStartedGuide.pdf"; WorkingDir: {app}\n')
    file.write('Name: "{group}\\Uninstall CASAL2"; Filename: "{uninstallexe}"\n')
    file.write('Name: "{commondesktop}\\run casal2"; Filename: "{app}\run casal2.lnk"; WorkingDir: "{app}"; Tasks: desktopicon\n')
    file.write('[Code]\n')    
    file.write('/////////////////////////////////////////////////////////////////////\n') 
    file.write('function GetUninstallString: string;\n') 
    file.write('var\n') 
    file.write('  sUnInstPath: string;\n') 
    file.write('  sUnInstallString: String;\n') 
    file.write('begin \n') 
    file.write('  Result := \'\';\n') 
    file.write('  sUnInstPath := ExpandConstant(\'Software\Microsoft\Windows\CurrentVersion\Uninstall\{#emit SetupSetting("AppId")}_is1\'); \n') 
    file.write('  sUnInstallString := \'\';\n') 
    file.write('  if not RegQueryStringValue(HKLM, sUnInstPath, \'UninstallString\', sUnInstallString) then \n') 
    file.write('    RegQueryStringValue(HKCU, sUnInstPath, \'UninstallString\', sUnInstallString);\n') 
    file.write('  Result := sUnInstallString;\n') 
    file.write('end;\n') 
    file.write('/////////////////////////////////////////////////////////////////////\n') 
    file.write('function IsUpgrade: Boolean;\n') 
    file.write('begin\n') 
    file.write('  Result := (GetUninstallString() <> \'\');\n') 
    file.write('end;\n') 
    file.write('/////////////////////////////////////////////////////////////////////\n') 
    file.write('function InitializeSetup: Boolean;\n') 
    file.write('var\n') 
    file.write('  V: Integer;\n') 
    file.write('  iResultCode: Integer;\n') 
    file.write('  sUnInstallString: string;\n') 
    file.write('begin\n') 
    file.write('  Result := True; // in case when no previous version is found\n') 
    file.write('  if RegValueExists(HKEY_LOCAL_MACHINE,\'Software\Microsoft\Windows\CurrentVersion\Uninstall\{#emit SetupSetting("AppId")}_is1\', \'UninstallString\') then  \n') 
    file.write('  begin \n') 
    file.write('    V := MsgBox(ExpandConstant(\'Hey! An old version of casal2 was detected. Do you want to uninstall it?\'), mbInformation, MB_YESNO); //Custom Message if App installed\n') 
    file.write('    if V = IDYES then\n') 
    file.write('    begin\n') 
    file.write('      sUnInstallString := GetUninstallString();\n') 
    file.write('      sUnInstallString :=  RemoveQuotes(sUnInstallString);\n') 
    file.write('      Exec(ExpandConstant(sUnInstallString), \'\', \'\', SW_SHOW, ewWaitUntilTerminated, iResultCode);\n') 
    file.write('      Result := True; //if you want to proceed after uninstall\n') 
    file.write('      //Exit; //if you want to quit after uninstall\n') 
    file.write('    end\n') 
    file.write('    else\n') 
    file.write('      Result := False; //when older version present and not uninstalled\n') 
    file.write('  end;\n') 
    file.write('end;    \n')   
    file.write('//////////////////////// \n')
    file.write('function NeedsAddPath(Param: string): boolean;\n')
    file.write('var\n')
    file.write('  OrigPath: string;\n')
    file.write('  ParamExpanded: string;\n')
    file.write('begin\n')
    file.write('  //expand the setup constants like {app} from Param\n')
    file.write('  ParamExpanded := ExpandConstant(Param);\n')
    file.write('  if not RegQueryStringValue(HKEY_LOCAL_MACHINE,\n')
    file.write('    \'SYSTEM\CurrentControlSet\Control\Session Manager\Environment\',\n')
    file.write('    \'Path\', OrigPath)\n')
    file.write('  then begin\n')
    file.write('    Result := True;\n')
    file.write('    exit;\n')
    file.write('  end;\n')
    file.write('  // look for the path with leading and trailing semicolon and with or without \ ending\n')
    file.write('  // Pos() returns 0 if not found\n')
    file.write('  Result := Pos(\';\' + UpperCase(ParamExpanded) + \';\', \';\' + UpperCase(OrigPath) + \';\') = 0;  \n')
    file.write('  if Result = True then\n')
    file.write('     Result := Pos(\';\' + UpperCase(ParamExpanded) + \'\;\', \';\' + UpperCase(OrigPath) + \';\') = 0; \n')
    file.write('end;\n')
    file.write('[Run]\n')
    file.write('Filename: {app}\README.txt; Description: View the README file; Flags: postinstall shellexec skipifsilent\n')
    file.close()

    if not os.path.exists("bin\\windows\\installer"):
      os.makedirs("bin\\windows\\installer")
    os.system('"C:\\Program Files (x86)\\Inno Setup 5\\ISCC.exe" /Obin\\windows\\installer\\ config.iss')

