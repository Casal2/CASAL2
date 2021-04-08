import os
import sys
import os.path
import subprocess
import fileinput
import distutils

sys.path.insert(0, "buildtools/classes")
EX_OK = getattr(os, "EX_OK", 0)

# These are CASAL2 specific python objects
from System import *
from Globals import *
from Builder import *
from Documentation import *
from Archiver import *
from ModelRunner import *
from DebBuilder import *
from CommandLineInterface import *

"""
Get the build information from the user
"""
def start():
	cli = CommandLineInterface()
	if not cli.parse():
  		cli.print_usage()
  		return False
      
	print("*************************************************************************")
	print("*************************************************************************")
	print(f"--> Starting {Globals.build_target_} Build")
	build_target = Globals.build_target_
	build_parameters = Globals.build_parameters_
	if build_target in Globals.allowed_build_types_:
		code_builder = MainCode()
		return code_builder.start()
	elif build_target == "library":
		if not build_parameters in Globals.allowed_library_parameters_:
			return Globals.PrintError(f"Library build parameter {build_parameters} is not valid")
		print("--> Starting " + Globals.build_target_ + " Build")
		code_builder = MainCode()
		return code_builder.start(build_library=True)			
	elif build_target == "frontend":
		print("--> Starting " + Globals.build_target_ + " Build")
		code_builder = FrontEnd()
		return code_builder.start()
	elif build_target == "archive":	
		archive_builder = Archiver()
		return archive_builder.start(build_parameters)
	elif build_target == "thirdparty" or build_target == "thirdpartylean":
		code_builder = ThirdPartyLibraries()
		return code_builder.start()
	elif build_target == "documentation":
		documentation_builder = Documentation()
		return documentation_builder.start()
	elif build_target == "modelrunner":
		model_runner = ModelRunner()
		return model_runner.start()
	elif build_target == "clean":
		cleaner = Cleaner()
		return cleaner.clean()	
	elif build_target == "cleancache":
		cleaner = Cleaner()
		return cleaner.clean_cache()
	elif build_target == "cleanall":
		cleaner = Cleaner()
		return cleaner.clean_all()
	elif build_target == "rlibrary":
		"""
			r_path = '' 
			if Globals.operating_system_ == 'windows':
			print( "find windows R")
			r_path = system_info.find_exe_path('R.exe') 
			else:
			r_path = system_info.find_exe_path('R')  
			print( "R path = " + r_path  )
			if r_path == '':
			return Globals.PrintError("R is not in the current path please add R to your path.")  
		"""
		rlibrary = Rlibrary()
		return rlibrary.start()
	elif build_target == "installer":
		if Globals.operating_system_ == 'linux':
			return Globals.PrintError('Building Windows installer under linux is not supported')		
		installer = Installer()
		return installer.start()

	elif build_target == "deb":
		if Globals.operating_system_ == 'windows':
			return Globals.PrintError('Building linux .deb under Windows is not supported')
		deb_builder = DebBuilder()
		return deb_builder.start(build_parameters)
	return False # Default return from this, we didn't find a run mode
  
"""
This is the entry point in to our build system
"""
system_info = SystemInfo()
if not system_info.set_required_tools_paths():
  system_info.reset_original_path()
  exit(1)

exit_code = 0
if not start():
  exit_code = 1
  
system_info.reset_original_path()
print("--> Finished")
exit(exit_code)  
