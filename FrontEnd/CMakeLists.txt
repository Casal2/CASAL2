cmake_minimum_required(VERSION 2.8)
project(CASAL2)

# Variables
SET(EXE_NAME "casal2")

# Change the output folders for our build to something cleaner and platform dependent
SET(CMAKE_ROOT   bin/cmake)

STRING(REPLACE "BuildSystem" "FrontEnd" PROJECT_HOME_DIRECTORY ${CMAKE_HOME_DIRECTORY})
STRING(REPLACE "FrontEnd" "CASAL2" CASAL2_HOME_DIRECTORY ${CMAKE_HOME_DIRECTORY})

# GLOBAL BUILD VARIABLES AND LIBRARIES
SET(COMPILE_OPTIONS "-O2 -g0 -Wall -Werror -fmessage-length=0 -Wno-unused-local-typedefs")

# Set variables based on OS
IF (WIN32)
 ## WINDOWS BUILD VARIABLES AND LIBRARIES
 SET(buildOS windows) 
 SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -D__GNUDOS__ -std=c++11")
 
ELSE (WIN32)
 ## LINUX BUILD VARIABLES AND LIBRARIES
 SET(buildOS linux)
 SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -std=c++11 -pthread")
 SET(thirdPartyLibraries ${thirdPartyLibraries} dl pthread rt)
ENDIF ()

# BUILD PARAMETERS
MESSAGE("COMPILE_OPTIONS: ${COMPILE_OPTIONS}")

INCLUDE_DIRECTORIES(SYSTEM "${CMAKE_HOME_DIRECTORY}/bin/${buildOS}/thirdparty/include")
INCLUDE_DIRECTORIES("${PROJECT_HOME_DIRECTORY}/source")
INCLUDE_DIRECTORIES("${CASAL2_HOME_DIRECTORY}/source")
MESSAGE("CASAL2 Folder: ${CASAL2_HOME_DIRECTORY}/source")

# This snippet of code will find all of our source and test files
# and auto-populate variables with them for the build
FILE(GLOB_RECURSE sourceFiles ${PROJECT_HOME_DIRECTORY}/source/*.cpp)
MESSAGE("SOURCE FILES: ${sourceFiles}")

SET(LINK_OPTIONS "-pthread")


IF(WIN32)
  MESSAGE("Building ICON")
  ADD_CUSTOM_TARGET(icon ALL COMMAND windres --output "${CMAKE_HOME_DIRECTORY}/icon.o" --input "${CMAKE_HOME_DIRECTORY}/../BuildSystem/casal2.rc")
  MESSAGE("Icon Built")
  SET(LINK_OPTIONS "-static ${CMAKE_HOME_DIRECTORY}/icon.o")  
ENDIF(WIN32)
ADD_EXECUTABLE(${EXE_NAME} ${sourceFiles} )


SET_TARGET_PROPERTIES(${EXE_NAME} PROPERTIES COMPILE_FLAGS ${COMPILE_OPTIONS} LINK_FLAGS ${LINK_OPTIONS})
IF(WIN32)
  ADD_DEPENDENCIES(${EXE_NAME} version icon)
ELSE(WIN32)
  TARGET_LINK_LIBRARIES(${EXE_NAME} ${thirdPartyLibraries})
ENDIF(WIN32)
