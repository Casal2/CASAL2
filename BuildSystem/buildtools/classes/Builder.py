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
import importlib.util

import Globals
from Version import *

EX_OK = getattr(os, "EX_OK", 0)

"""
This class is responsible for building the main code base of the project
"""
class MainCode:
    cmake_compiler_ = ""
    output_directory_ = ""

    def start(self, build_library=False):
        start = time.time()
        print("--> Starting build of the main code base")
        print(f"--> Build configuration {Globals.build_target_} : {Globals.build_parameters_}")
        print(f"--> Operating System: {Globals.operating_system_}")

        # [ 'Unix', 'MinGW' ][ Globals.operating_system_ == "windows" ]
        self.cmake_compiler_ = Globals.cmake_compiler_
        print(f"--> CMake Compiler: {Globals.cmake_compiler_}")

        # Check to see if the third party libraries have been built
        third_party_dir = f"bin/{Globals.operating_system_}_{Globals.compiler_}/thirdparty"
        if not os.path.exists(third_party_dir):
            return Globals.PrintError("Third party libraries have not been built. Please build these first with thirdparty argument")

        self.output_directory_ = f"bin/{Globals.operating_system_}_{Globals.compiler_}/{Globals.build_target_}"
        if Globals.build_parameters_ != "":
            self.output_directory_ += f"_{Globals.build_parameters_}"

        if not os.path.exists(self.output_directory_):
            os.makedirs(self.output_directory_)
        print(f"--> Target output directory: {self.output_directory_}")
        os.chdir(self.output_directory_)

        if Globals.build_target_.upper() == "TEST":  # Handle new keyword TEST in CMake v3
            Globals.build_target_ = "TESTMODE"
        if Globals.build_parameters_.upper() == "TEST":  # Handle new keyword TEST in CMake v3 for library builds
            Globals.build_parameters_ = "TESTMODE"

        print("--> Preparing CMake command")
        build_string = 'cmake ' + self.cmake_compiler_ + ' -D' + Globals.build_target_.upper() + '=1'
        if Globals.build_parameters_ != "":
            build_string += ' -D' + Globals.build_parameters_.upper() + '=1'
        build_string += ' ../../../../CASAL2/'

        print("--> CMake command: " + build_string)
        if os.system(build_string) != EX_OK:
            return Globals.PrintError("Failed to execute cmake successfully to rebuild the make files")

        print("--> Build main code base")
        if os.system(Globals.make_command_) != EX_OK:
            return Globals.PrintError("Failed to build code base. Please see above for build error")

        elapsed = time.time() - start
        print('Compile finished in ' + str(round(elapsed, 2)) + ' seconds')
        return True


"""
This class is responsible for build the third party libraries part of the code base
"""
class ThirdPartyLibraries:
    input_directory = ""
    output_directory_ = ""
    include_directory = ""
    lib_directory = ""
    lib_debug_directory = ""
    lib_release_directory = ""
    lib_special_directory = ""

    def start(self):
        print("--> Starting build of the third party libraries")
        print("-- Operating System: " + Globals.operating_system_)
        print("-- Compiler: " + Globals.compiler_)

        print("-- Checking if output folder structure exists")
        self.output_directory_ = os.path.normpath(os.getcwd()) + "/bin/" + Globals.operating_system_ + "_" + Globals.compiler_ + "/thirdparty"
        if not os.path.exists(self.output_directory_):
            print("-- Creating output directory: " + self.output_directory_)
            os.makedirs(self.output_directory_)

        Globals.target_success_path_ = self.output_directory_ + '/'
        Globals.target_include_path_ = self.output_directory_ + '/include/'
        Globals.target_debug_lib_path_ = self.output_directory_ + '/lib/debug/'
        Globals.target_release_lib_path_ = self.output_directory_ + '/lib/release/'
        Globals.target_special_lib_path_ = self.output_directory_ + '/lib/special/'

        print("-- Creating any missing output folders")
        os.makedirs(Globals.target_success_path_, exist_ok=True)
        os.makedirs(Globals.target_include_path_, exist_ok=True)
        os.makedirs(Globals.target_debug_lib_path_, exist_ok=True)
        os.makedirs(Globals.target_release_lib_path_, exist_ok=True)
        os.makedirs(Globals.target_special_lib_path_, exist_ok=True)

        self.input_directory = "../ThirdParty/"
        third_party_list = os.listdir(self.input_directory)

        cwd = os.path.normpath(os.getcwd())
        build_module_name = "build"

        found_build = False
        for folder in third_party_list:
            # Do some simple checks to see if we want to process this folder
            if folder.startswith("."):
                continue
            if Globals.build_parameters_ != "" and Globals.build_parameters_ != folder.lower():
                continue
            if folder.startswith("test-") and Globals.build_parameters_ == "":
                print('--> Skipping library ' + folder + ' (test library)')
                continue

            # load the version number so we can handle upgrades
            library_version = str(-1.0)
            success_file = self.output_directory_ + "/" + folder + ".success"
            if os.path.exists(success_file):
                f = open(success_file, "r")
                library_version = f.readline().lstrip().rstrip().replace('\n', '')
                f.close()

            found_build = True
            target_folder = "../ThirdParty/" + folder
            success = False

            os.chdir(target_folder)
            sys.path.append(os.path.normpath(os.getcwd()))

            """
            # Handle loading the windows file and building this on windows
            """
            file_name = ""
            if Globals.operating_system_ == "windows":
                file_name = "windows"
            elif Globals.operating_system_ == "linux":
                file_name = "linux"

            if Globals.compiler_ == "gcc":
                file_name += "_gcc"
            elif Globals.compiler_ == "msvc":
                file_name += "_msvc"

            module_name = file_name
            file_name += ".py"

            if not os.path.exists(file_name):
                return Globals.PrintError('Third party library ' + folder + ' does not have a ' + file_name + ' file.\nThis file is required to build this library')
            print(os.path.normpath(os.getcwd()))
            builder = importlib.import_module(module_name).Builder()

            if os.path.exists(success_file) and Globals.build_parameters_ == "" and hasattr(builder, 'version_') and str(library_version) == str(builder.version_) and str(library_version) != str(-1.0):
                print('--> Skipping library ' + folder + ' (version already installed)')
                success = True
            else:
                print("")
                do_build = False
                if Globals.build_target_ == "thirdpartylean" and hasattr(builder, 'is_lean') and builder.is_lean():
                    do_build = True
                elif Globals.build_target_ == "thirdparty":
                    do_build = True

                if do_build:
                    print("--> Building Library: " + folder)
                    if not hasattr(builder, 'start'):
                        return Globals.PrintError('Third party library ' + folder + ' does not have a valid start() method in ' + file_name)

                    success = builder.start()
                    if success:
                        os.system(f"echo {builder.version_} > {Globals.target_success_path_}{folder}.success")
                        print("")
                    else:
                        return Globals.PrintError("Third party library " + folder + " had an error during the build. Check log files for more information")
            del sys.modules[module_name]

            sys.path.pop()
            os.chdir(cwd)
            if not success:
                return False

        if not found_build and Globals.build_parameters_ != "":
            return Globals.PrintError("Third party library " + Globals.build_parameters_ + " does not exist")

        print("")
        print("--> All third party libraries have been built successfully")
        return True


"""
This class is responsible for build the front end application
"""
class FrontEnd:
    def start(self):
        start = time.time()
        print("--> Starting build of the front end code base")
        print("--> Build configuration " + Globals.build_target_ + " : " + Globals.build_parameters_)
        print("--> Operating System: " + Globals.operating_system_)

        self.cmake_compiler_ = ['Unix', 'MinGW'][Globals.operating_system_ == "windows"]
        print("--> CMake Compiler: " + self.cmake_compiler_)

        # Check to see if the third party libraries have been built
        # Check to see if the third party libraries have been built
        third_party_dir = f"bin/{Globals.operating_system_}_{Globals.compiler_}/thirdparty"
        if not os.path.exists(third_party_dir):
            return Globals.PrintError("Third party libraries have not been built. Please build these first with thirdparty argument")

        self.output_directory_ = f"bin/{Globals.operating_system_}_{Globals.compiler_}/{Globals.build_target_}"

        if Globals.build_parameters_ != "":
            self.output_directory_ += "_" + Globals.build_parameters_

        if not os.path.exists(self.output_directory_):
            os.makedirs(self.output_directory_)
        print("--> Target output directory: " + self.output_directory_)

        os.chdir(self.output_directory_)

        print('--> Preparing CMake command')
        build_string = 'cmake -G "' + self.cmake_compiler_ + ' Makefiles" ../../../../FrontEnd'
        print("--> CMake command: " + build_string)
        if os.system(build_string) != EX_OK:
            return Globals.PrintError("Failed to execute cmake successfully to rebuild the make files")

        print("--> Build main code base")
        if Globals.operating_system_ == "windows":
            if os.system("mingw32-make") != EX_OK:
                return Globals.PrintError("Failed to build code base. Please see above for build error")
        else:
            if os.system("make") != EX_OK:
                return Globals.PrintError("Failed to build code base. Please see above for build error")

        elapsed = time.time() - start
        print('Compile finished in ' + str(round(elapsed, 2)) + ' seconds')

        return True


"""
This class is responsible for cleaning the build folders
"""
class Cleaner:
    def clean(self):
        print('--> Starting clean of Casal2 build files only (not third party)')
        for build_type in Globals.allowed_build_types_:
            for param in Globals.allowed_build_parameters_:
                build_directory = os.path.normpath(os.getcwd()) + f"/bin/{Globals.operating_system_}_{Globals.compiler_}/{build_type}"
                if param != "":
                    build_directory = build_directory + "_" + param
                print(f"-- Build Directories: {build_directory}") 
                if (os.path.exists(build_directory)):
                    print('--> Deleting folder: ' + build_directory)
                    shutil.rmtree(build_directory)
                else:
                    print('--> Folder doesn\'t exist: ' + build_directory)
        return True

    def clean_cache(self):
        print("--> Asking third party libraries to remove all temporary files")
        print("--> ##### NOT YET IMPLEMENTED #####")

    def clean_all(self):
        print('--> Starting clean of all Casal2 build files (including third party)')
        print('--> This removes ALL binary files and ALL pre-built Casal2 files')
        build_directory = os.path.normpath(os.getcwd()) + f"/bin/"
        print(f"-- Build Directories: {build_directory}") 
        if (os.path.exists(build_directory)):
            shutil.rmtree(build_directory)
        build_directory = os.path.normpath(os.getcwd()) + f"/Casal2/"
        print(f"-- Build Directories: {build_directory}") 
        if (os.path.exists(build_directory)):
            shutil.rmtree(build_directory)
        return True
