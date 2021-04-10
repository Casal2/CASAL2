"""
This class is responsible for creating the Version.h file in the source directory
and returning the version parameters used by the documentation generator
"""
import os
import sys
import subprocess
import os.path
import shutil
import fileinput
import re
import time
import pytz
from datetime import datetime, date

import Globals

class Version:
	def __init__(self):
		print("--> Starting Version Class")

	def create_version_header(self, display_output=False):
		print("--> Creating Version.h from Git Information")
		
		# Build the Version.h file
		if Globals.git_path_ == '':
			print("[WARNING] - No Git was found. Cannot create Version.h file")
			return True

		p = subprocess.Popen(['git', '--no-pager', 'log', '-n', '1', '--pretty=format:%H%n%h%n%ci' ], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		out, err = p.communicate()
		lines = out.decode('utf-8').split('\n')
		if len(lines) != 3:
			return Globals.PrintError('Format printed by GIT did not meet expectations. Expected 3 lines but got ' + str(len(lines)))

		time_pieces = lines[2].split(' ')
		temp = ' '.join(time_pieces)
		local_time = datetime.strptime(temp, '%Y-%m-%d %H:%M:%S %z')
		utc_time   = local_time.astimezone(pytz.utc)

		version = '// This file is automatically built by the build system. Do not modify this file\n'
		version += '#ifndef VERSION_H_\n'
		version += '#define VERSION_H_\n'
		version += '#define SOURCE_CONTROL_REVISION ' + lines[0] + '\n'
		version += '#define SOURCE_CONTROL_DATE "' + utc_time.strftime('%Y-%m-%d') + '"\n'
		version += '#define SOURCE_CONTROL_YEAR "' + utc_time.strftime('%Y') + '"\n'
		version += '#define SOURCE_CONTROL_MONTH "' + utc_time.strftime('%B') + '"\n'
		version += '#define SOURCE_CONTROL_TIME "' + utc_time.strftime('%H:%M:%S') + '"\n'
		version += '#define SOURCE_CONTROL_VERSION "' + utc_time.strftime('%Y-%m-%d %H:%M:%S %Z') + ' (rev. ' + lines[1] + ')"\n'
		version += '#endif\n'

		if display_output:
			print(version)

		fo = open('../CASAL2/source/Version.h', 'w')
		fo.write(version)
		fo.close()
		return True