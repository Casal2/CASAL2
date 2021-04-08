import sys
import os

import Globals

class CommandLineInterface:
	def __init__(self):
		print('--> Spawning Command Line Interface Parser')

	def print_usage(self):
		#os.system( [ 'clear', 'cls' ][ os.name == 'nt' ] )
		print('###########################################################')
		print( '# CASAL2 Build System Usage                               #')
		print( '###########################################################')
		print( 'Usage:')
		print( 'doBuild <build_target> <build_parameter>')
		print( '')
		print( 'Valid Build Types:')
		print( '  debug - Build standalone debug executable')
		print( '  release - Build standalone release executable')
		print( '  test - Build standalone unit tests executable')
		print( '  documentation - Build the user manual')
		print( '  thirdparty - Build all required third party libraries')
		print( '  thirdpartylean - Build minimal third party libraries')
		print( '  clean - Remove any previous debug/release build information')
		print( '  cleanall - Remove all previous build information')
		print( '  archive - Build a zipped archive of the application.')
		print( '            Application is built using shared libraries')
		print( '            so a single casal2 exectable is created.')
		print( '  check - Do a check of the build system')
		print( '  rlibrary - Build the R library')
		print( '  modelrunner - Run the test suite of models')
		print( '  installer - Build an installer package')
		print( '  deb - Create linux .deb installer')
		print( '  library - Build shared library for use by front end application')
		print( '')
		print( 'Valid Build Parameters: (thirdparty only)')
		print( '  <libary name> - Target third party library to build or rebuild')
		print( '')
		return True

	def parse(self):
		print('--> Parsing Command Line Parameters')
		build_target = ""
		build_parameters = ""

		"""
		Handle build information already passed in
		"""
		if len(sys.argv) > 1 and len(str(sys.argv[1])) > 1:
			build_target = sys.argv[1]
		if len(sys.argv) > 2 and len(str(sys.argv[2])) > 1:
			build_parameters = sys.argv[2] 

		if build_target == "":
			return Globals.PrintError('Please provide a valid build target. Use doBuild help to see list');
		if not build_target.lower() in Globals.allowed_build_targets_:
			return Globals.PrintError(build_target + " is not a valid build target")

		build_target = build_target.lower()    
		if build_target == "help":
			return self.print_usage()
		if build_target == "check":
			print("--> All checks completed successfully")
			return True

		if build_parameters != "": 
			build_parameters = build_parameters.lower()

		Globals.build_target_ = build_target
		Globals.build_parameters_ = build_parameters

		print("-- Build target: " + Globals.build_target_)
		print("-- Build parameters: " + Globals.build_parameters_)

		if build_target != "" and not build_target in Globals.allowed_build_targets_:      
			return Globals.PrintError("Build target " + build_target + " is not valid")
		if Globals.build_target_ == "library" and not Globals.build_parameters_ in Globals.allowed_library_parameters_:
			return Globals.PrintError("Library build parameter" + Globals.build_parameters_ + " is not valid")	

		return True
