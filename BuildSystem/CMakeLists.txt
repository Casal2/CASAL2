cmake_minimum_required(VERSION 2.8)
project(CASAL2)

# Variables
SET(EXE_NAME "casal2")

# Change the output folders for our build to something cleaner and platform dependent
SET(CMAKE_ROOT   bin/cmake)

STRING(REPLACE "BuildSystem" "CASAL2" PROJECT_HOME_DIRECTORY ${CMAKE_HOME_DIRECTORY})

# GLOBAL BUILD VARIABLES AND LIBRARIES
IF (DEBUG)
 SET(COMPILE_OPTIONS "-O0 -g3 -Wall -Werror -fmessage-length=0 -Wno-unused-local-typedefs")
ELSEIF (RELEASE)
 SET(COMPILE_OPTIONS "-O2 -g0 -Wall -Werror -fmessage-length=0 -Wno-unused-local-typedefs")
ELSEIF (TEST)
 SET(COMPILE_OPTIONS "-DTESTMODE -O2 -g0 -Wall -Werror -fmessage-length=0 -Wno-unused-local-typedefs")
ELSE()
 SET(COMPILE_OPTIONS "-O2 -g0 -Wall -Werror -fmessage-length=0 -Wno-unused-local-typedefs")
ENDIF ()

# Set variables based on OS
IF (WIN32)
 ## WINDOWS BUILD VARIABLES AND LIBRARIES
 SET(buildOS windows) 
 SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -D__GNUDOS__ -std=c++11")
 
ELSE (WIN32)
 ## LINUX BUILD VARIABLES AND LIBRARIES
 SET(buildOS linux)
 SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -fPIC -std=c++11")
 SET(thirdPartyLibraries ${thirdPartyLibraries} pthread rt)
ENDIF ()

# BUILD PARAMETERS
IF (ADOLC)
 SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -DUSE_AUTODIFF -DUSE_ADOLC")
 SET(specialLibraries "-l:libadolc.a")
ELSEIF(BETADIFF)
 SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -DUSE_AUTODIFF -DUSE_BETADIFF")
 SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -Wno-unused-local-typedefs -Wno-write-strings -Wno-unused-variable -Wno-unused-but-set-variable -fpermissive -Wno-error")
 SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -iquote${CMAKE_HOME_DIRECTORY}/bin/${buildOS}/thirdparty/include/ADOLC18/SRC")
 SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -iquote${CMAKE_HOME_DIRECTORY}/bin/${buildOS}/thirdparty/include/ADOLC18/SRC/DRIVERS")
 SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -iquote${CMAKE_HOME_DIRECTORY}/bin/${buildOS}/thirdparty/include/ADOLC18/SRC/SPARSE")
 SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -iquote${CMAKE_HOME_DIRECTORY}/bin/${buildOS}/thirdparty/include/ADOLC18/SRC/TAPEDOC")
 SET(specialLibraries "-l:libbetadiff.a") 
ELSEIF(CPPAD)
 SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -DUSE_AUTODIFF -DUSE_CPPAD")
 IF (WIN32)
  SET(specialLibraries
   "ipopt"   
   "coinmumps"
   "coinmetis"
   "m"
   "gfortran"   
   "quadmath"
   "msvcrt"   
   "coinlapack"
   "coinblas"   
  )
 ELSE (WIN32)
  SET(specialLibraries   
   "quadmath"
   "dl"
   "ipopt"
   "coinmumps"
   "coinlapack"
   "coinblas"   
   "coinmetis"
   "m"
   "gfortran"   
   "dl"
  ) 
 ENDIF ()
ENDIF ()

MESSAGE("COMPILE_OPTIONS: ${COMPILE_OPTIONS}")
MESSAGE("SPECIAL_LIBRARIES: ${specialLibraries}")

INCLUDE_DIRECTORIES(SYSTEM "${CMAKE_HOME_DIRECTORY}/bin/${buildOS}/thirdparty/include")
INCLUDE_DIRECTORIES("${PROJECT_HOME_DIRECTORY}/source")

# This snippet of code will find all of our source and test files
# and auto-populate variables with them for the build
FILE(GLOB_RECURSE sourceFiles ${PROJECT_HOME_DIRECTORY}/source/*.cpp)
IF (NOT TEST)
  FILE(GLOB_RECURSE testFiles ${PROJECT_HOME_DIRECTORY}/source/*.Test.cpp)
  list(REMOVE_ITEM sourceFiles ${testFiles})
ENDIF ()

# Get a list of our third party libraries
IF (RELEASE)
 SET(thirdPartyFolder "bin/${buildOS}/thirdparty/lib/release/")
ELSE (RELEASE)
 SET(thirdPartyFolder "bin/${buildOS}/thirdparty/lib/debug/")
ENDIF ()
MESSAGE("Third Party Folder: ${thirdPartyFolder}")
LINK_DIRECTORIES("${CMAKE_HOME_DIRECTORY}/${thirdPartyFolder}")
LINK_DIRECTORIES("${CMAKE_HOME_DIRECTORY}/bin/${buildOS}/thirdparty/lib/special/")

FILE(GLOB thirdPartyLibrariesRaw RELATIVE ${CMAKE_HOME_DIRECTORY} ${thirdPartyFolder}*.a)
FOREACH(library ${thirdPartyLibrariesRaw})
  STRING(REPLACE ${thirdPartyFolder} "" shortLibrary ${library})
  SET(thirdPartyLibraries ${thirdPartyLibraries} -l:${shortLibrary})
ENDFOREACH()

FOREACH(library ${specialLibraries})
SET(thirdPartyLibraries ${thirdPartyLibraries} ${library})
ENDFOREACH()

SET(LINK_OPTIONS " ")

IF (LIBRARY)
  ADD_LIBRARY(${EXE_NAME} SHARED ${sourceFiles} )
  SET(LINK_OPTIONS "-static")
ELSE(LIBRARY)
  IF(NOT TEST)
  	IF(WIN32)
  	  MESSAGE("Building ICON")
  	  ADD_CUSTOM_TARGET(icon ALL COMMAND windres --output "${CMAKE_HOME_DIRECTORY}/icon.o" --input "${CMAKE_HOME_DIRECTORY}/casal2.rc")
  	  MESSAGE("Icon Built")
  	  SET(LINK_OPTIONS "-static ${CMAKE_HOME_DIRECTORY}/icon.o")  
  	ENDIF(WIN32)
  ENDIF()
  MESSAGE("Link Options: ${LINK_OPTIONS}")
  MESSAGE("Third party libraries: ${thirdPartyLibraries}")

  ADD_EXECUTABLE(${EXE_NAME} ${sourceFiles} )
ENDIF(LIBRARY)

SET_TARGET_PROPERTIES(${EXE_NAME} PROPERTIES COMPILE_FLAGS ${COMPILE_OPTIONS} LINK_FLAGS ${LINK_OPTIONS})
TARGET_LINK_LIBRARIES(${EXE_NAME} ${thirdPartyLibraries})
IF(NOT TEST)
	IF(WIN32)
	  ADD_DEPENDENCIES(${EXE_NAME} version icon)
	ENDIF(WIN32)
ENDIF()