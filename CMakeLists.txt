cmake_minimum_required(VERSION 2.6)
project(iSAM)

# Change the output folders for our build to something cleaner and platform dependent
SET(CMAKE_ROOT   build/cmake)

# This snippet of code will find all of our source and test files
# and auto-populate variables with them for the build
FILE(GLOB_RECURSE sourceFiles RELATIVE ${CMAKE_HOME_DIRECTORY} source/*.cpp)
FILE(GLOB_RECURSE testFiles RELATIVE ${CMAKE_HOME_DIRECTORY} source/*.Test.cpp)
# Remove the test files from the source files list
list(REMOVE_ITEM sourceFiles ${testFiles})

# GLOBAL BUILD VARIABLES AND LIBRARIES
IF (DEBUG)
 SET(COMPILE_OPTIONS "-O0 -g3 -Wall -Werror -fmessage-length=0")
ELSEIF (RELEASE)
 SET(COMPILE_OPTIONS "-O3 -g0 -Wall -Werror -fmessage-length=0")
ENDIF ()

# Set variables based on OS
IF (WIN32)
 ## WINDOWS BUILD VARIABLES AND LIBRARIES
 SET(buildOS win32) 
 
ELSEIF (LINUX)
 ## LINUX BUILD VARIABLES AND LIBRARIES
 SET(buildOS linux)
 
ENDIF ()

# Get a list of our third party libraries
FILE(GLOB children RELATIVE ${CMAKE_HOME_DIRECTORY} build/${buildOS}/thirdparty/*)
SET(thirdPartyDirs "")

# Now check for folders to use
FOREACH(child ${children})
 IF (IS_DIRECTORY ${CMAKE_HOME_DIRECTORY}/${child})   
  SET(thirdPartyDirs ${thirdPartyDirs} ${CMAKE_HOME_DIRECTORY}/${child})
 ENDIF()
ENDFOREACH()



