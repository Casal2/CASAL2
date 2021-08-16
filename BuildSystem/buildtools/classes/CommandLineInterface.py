import sys
import os

import Globals


class CommandLineInterface:
    def __init__(self):
        print('--> Spawning Command Line Interface Parser')

    def print_usage(self):
        #os.system( [ 'clear', 'cls' ][ os.name == 'nt' ] )
        print('###############################################################################')
        print('# Casal2 Build System Usage                                                   #')
        print('###############################################################################')
        print('Usage: doBuild <build_target> <argument>')
        print('')
        print('  help        - Print out the doBuild help (this output)')
        print('  check       - Do a check of the build system')
        print('  clean       - Remove debug/release build files')
        print('  clean_all   - Remove ALL build files and ALL prebuilt binaries')
        print('  version     - Build the current version files for C++, R, and LaTeX')
        print('')
        print('Build required libraries (DLLs/shared objects for Casal2)')
        print('  thirdparty  - Build the third party libraries')
        print('                <option> Optionally specify the target third party library to')
        print('                         build, either adolc or betadiff (default is none)')
        print('')
        print('Build development and test versions (for development only)')
        print('  release     - Build stand-alone release executable')
        print('                <option> Optionally specify the target third party library to')
        print('                         build, either adolc or betadiff (default is none)')
        print('  debug       - Build stand-alone debug executable')
        print('                <option> Optionally specify the target third party library to')
        print('                         build, either adolc or betadiff (default is none)')
        print('  test        - Build stand-alone unit tests executable')
        print('  unittests   - Run the unit tests')
        print('  modelrunner - Run the test suite (requires "test" to have been built)')
        print('')
        print('Build the Casal2 end-user application')
        print('  library     - Build shared library for use by front end application')
        print('                <argument> Required argument to specify the target library to ')
        print('                         build: release, adolc, betadiff, or test')
        print('  frontend    - Build Casal2 front end application')
        print('')
        print('Create the archive, R Library, documentation, and the installers')
        print('  documentation - Build the Casal2 user manual')
        print('  rlibrary      - Create the R library')
        print('  archive       - Create a zipped archive of the Casal2 application.')
        print('                  <true> - if specified build skips everything but frontend')
        print('  installer     - Create the Microsoft Windows installer package')
        print('  deb           - Create Linux .deb installer')
        print('')
        print('')
        return True

# Building the Casal2 applications

# The fast way
# dobuild thirdparty
# dobuild archive
# dobuild installer
# dobuild deb

# the slow way
# thirdparty
# library release
# library test
# library adolc
# library betadiff
# frontend
# installer
# deb

    def parse(self):
        print('--> Parsing Command Line Parameters')
        build_target = ""
        build_parameters = ""

        ###
        # Handle build information already passed in
        ###
        if len(sys.argv) > 1 and len(str(sys.argv[1])) > 1:
            build_target = sys.argv[1]
        if len(sys.argv) > 2 and len(str(sys.argv[2])) > 1:
            build_parameters = sys.argv[2]

        if build_target == "":
            return Globals.PrintError('Please provide a valid build target. Use doBuild help to see list')
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
