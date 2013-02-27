cmake_minimum_required(VERSION 2.6)
project(iSAM)

# Variables
SET(EXE_NAME "iSAM")

# Change the output folders for our build to something cleaner and platform dependent
SET(CMAKE_ROOT   build/cmake)

MESSAGE("CMAKE_HOME_DIRECTORY: ${CMAKE_HOME_DIRECTORY}")

# GLOBAL BUILD VARIABLES AND LIBRARIES
IF (DEBUG)
 SET(COMPILE_OPTIONS "-O0 -g3 -Wall -Werror -fmessage-length=0 -DBOOST_THREAD_USE_LIB=1")
ELSEIF (RELEASE)
 SET(COMPILE_OPTIONS "-O3 -g0 -Wall -Werror -fmessage-length=0 -DBOOST_THREAD_USE_LIB=1")
ENDIF ()

IF (ADMB)
  SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -DADMB")
ENDIF (ADMB)

# Figure out what compiler version we're using
EXECUTE_PROCESS( COMMAND gcc -dumpversion GCC_VERSION)
MESSAGE("Compiler Version: ${GCC_VERSION}")
STRING(SUBSTRING "${GCC_VERSION}" 0 3 GCC_VERSION)
MESSAGE("Compiler Version: ${GCC_VERSION}")

# Set variables based on OS
IF (WIN32)
 ## WINDOWS BUILD VARIABLES AND LIBRARIES
 SET(buildOS win32) 
 SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -std=c++11 -std=gnu++11")
 
ELSE (WIN32)
 ## LINUX BUILD VARIABLES AND LIBRARIES
 SET(buildOS linux)
 SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -std=c++0x -std=gnu++0x")
 
ENDIF ()

MESSAGE("COMPILE_OPTIONS: ${COMPILE_OPTIONS}")

INCLUDE_DIRECTORIES("${CMAKE_HOME_DIRECTORY}/source")
INCLUDE_DIRECTORIES("${CMAKE_HOME_DIRECTORY}/build/${buildOS}/thirdparty/include")
LINK_DIRECTORIES("${CMAKE_HOME_DIRECTORY}/build/${buildOS}/thirdparty/lib/")

# This snippet of code will find all of our source and test files
# and auto-populate variables with them for the build
FILE(GLOB_RECURSE sourceFiles RELATIVE ${CMAKE_HOME_DIRECTORY} source/*.cpp)
FILE(GLOB_RECURSE testFiles RELATIVE ${CMAKE_HOME_DIRECTORY} source/*.Test.cpp)
# Remove the test files from the source files list
list(REMOVE_ITEM sourceFiles ${testFiles})

# Get a list of our third party libraries
FILE(GLOB thirdPartyLibrariesRaw RELATIVE ${CMAKE_HOME_DIRECTORY} build/${buildOS}/thirdparty/lib/*.a)
MESSAGE("THIRD PARTY PATH: ${CMAKE_HOME_DIRECTORY}/build/${buildOS}/thirdparty/lib/")
MESSAGE("THIRD PARTY LIBS: ${thirdPartyLibrariesRaw}")
FOREACH(library ${thirdPartyLibrariesRaw})
  STRING(REPLACE "build/${buildOS}/thirdparty/lib/" "" shortLibrary ${library})
  SET(thirdPartyLibraries ${thirdPartyLibraries} -l:${shortLibrary})
ENDFOREACH()
MESSAGE("THIRD PARTY LIBS: ${thirdPartyLibraries}")

SET(LINK_OPTIONS " ")

ADD_EXECUTABLE(${EXE_NAME} ${sourceFiles} )
SET_TARGET_PROPERTIES(${EXE_NAME} PROPERTIES COMPILE_FLAGS ${COMPILE_OPTIONS} LINK_FLAGS ${LINK_OPTIONS})
TARGET_LINK_LIBRARIES(${EXE_NAME} ${thirdPartyLibraries})



