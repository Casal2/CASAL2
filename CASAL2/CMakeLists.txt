cmake_minimum_required(VERSION 3.12)
project(CASAL2)

# Variables
SET(EXE_NAME "casal2")

# Build our OUTPUT path so we can use BuildSystem/bin/<output_path>/ for compiling
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  # using Clang
	SET(OUTPUT_PATH "clang")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # using GCC
	SET(OUTPUT_PATH "gcc")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # using Visual Studio C++
	SET(OUTPUT_PATH "msvc")
endif()

if (WIN32)
	SET(OUTPUT_PATH "windows_${OUTPUT_PATH}")
elseif (UNIX)
	SET(OUTPUT_PATH "linux_${OUTPUT_PATH}")
endif()
MESSAGE("-- Output Path: ${OUTPUT_PATH}")

# Change the output folders for our build to something cleaner and platform dependent
#SET(CMAKE_ROOT   bin/cmake)

# Build some path variables so we can easily find our third party libraries and code
SET(PROJECT_HOME_DIRECTORY ${CMAKE_HOME_DIRECTORY})
STRING(REPLACE "/CASAL2/CASAL2" "/CASAL2/BuildSystem/" BUILD_SYSTEM_DIRECTORY ${CMAKE_HOME_DIRECTORY})
MESSAGE("-- Source Root Folder: ${CMAKE_HOME_DIRECTORY}")
MESSAGE("-- BuildSystem Folder: ${BUILD_SYSTEM_DIRECTORY}")

SET(THIRD_PARTY_PATH "${BUILD_SYSTEM_DIRECTORY}bin/${OUTPUT_PATH}/thirdparty")
MESSAGE("-- Thirdparty Library Path ${THIRD_PARTY_PATH}")

# Set the third party include folder as an include
INCLUDE_DIRECTORIES(SYSTEM "${CMAKE_HOME_DIRECTORY}/source") # TODO: Remove later
INCLUDE_DIRECTORIES(SYSTEM "${THIRD_PARTY_PATH}/include")

# GLOBAL BUILD VARIABLES AND LIBRARIES
SET(COMPILE_OPTIONS "-Wall -Werror -fmessage-length=0 -Wno-unused-local-typedefs")
IF (DEBUG EQUAL 1)
	MESSAGE("-- Build Type: Debug")
 	SET(COMPILE_OPTIONS "-O0 -g -static ${COMPILE_OPTIONS}")
ELSEIF (RELEASE EQUAL 1)
	MESSAGE("-- Build Type: Release")
 	SET(COMPILE_OPTIONS "-O3 -g0 ${COMPILE_OPTIONS}")
ELSEIF (TESTMODE EQUAL 1)
	MESSAGE("-- Build Type: Unit Test")
 	SET(COMPILE_OPTIONS "-DTESTMODE -O3 -g0 ${COMPILE_OPTIONS}")
ELSE()
 	SET(COMPILE_OPTIONS "-O3 -g0 ${COMPILE_OPTIONS}")
ENDIF ()

# Set variables based on OS
IF (WIN32)
	MESSAGE("--> Building on Windows")
	IF (MSVC)
		SET(COMPILE_OPTIONS "/std:c++17 /MP /O2 /GT /GL")
	ELSE(MSVC)
		SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -D__MINGW32__ -D__GNUDOS__ -std=c++17")
 		SET(thirdPartyLibraries ${thirdPartyLibraries} pthread ws2_32 gdi32 comctl32 user32 winmm ws2_32 imm32)
	ENDIF ()
ELSEIF (UNIX)
	MESSAGE("--> Building on Linux")
 	SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -fPIC -std=c++17")
 	SET(thirdPartyLibraries ${thirdPartyLibraries} lapack blas pthread rt)
ENDIF ()

# Set compiler options for using saniizers
IF (ASAN)
	SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -static-libasan -fsanitize=address -fno-omit-frame-pointer")
	SET(thirdPartyLibraries asan ${thirdPartyLibraries} )
ELSEIF(TSAN)
	SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -static-libtsan -fsanitize=thread -fno-omit-frame-pointer")
	SET(thirdPartyLibraries tsan ${thirdPartyLibraries})
ELSEIF(MSAN)
	SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -static-libmsan -fsanitize=memory -fno-omit-frame-pointer")
	SET(thirdPartyLibraries msan ${thirdPartyLibraries})
ENDIF()
MESSAGE("-- Compiler Options: ${COMPILE_OPTIONS}")

# Now we want to see if we're building an auto-diff library or something of that nature.
# BUILD PARAMETERS
IF (ADOLC)
 SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -DUSE_AUTODIFF -DUSE_ADOLC")
 SET(specialLibraries "-l:libadolc.a")
ELSEIF (BETADIFF)
 SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -DUSE_AUTODIFF -DUSE_BETADIFF")
 SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -Wno-unused-local-typedefs -Wno-write-strings -Wno-unused-variable -Wno-unused-but-set-variable -fpermissive -Wno-error")
 SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -iquote${BUILD_SYSTEM_DIRECTORY}/bin/${OUTPUT_PATH}/thirdparty/include/ADOLC18/SRC")
 SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -iquote${BUILD_SYSTEM_DIRECTORY}/bin/${OUTPUT_PATH}/thirdparty/include/ADOLC18/SRC/DRIVERS")
 SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -iquote${BUILD_SYSTEM_DIRECTORY}/bin/${OUTPUT_PATH}/thirdparty/include/ADOLC18/SRC/SPARSE")
 SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -iquote${BUILD_SYSTEM_DIRECTORY}/bin/${OUTPUT_PATH}/thirdparty/include/ADOLC18/SRC/TAPEDOC")
 SET(specialLibraries "-l:libbetadiff.a")
ELSEIF (CPPAD)
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
 ELSEIF (UNIX)
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
MESSAGE("SPECIAL_LIBRARIES: ${specialLibraries}")

# This snippet of code will find all of our source and test files
# and auto-populate variables with them for the build
FILE(GLOB_RECURSE sourceFiles ${PROJECT_HOME_DIRECTORY}/source/*.cpp)
IF (NOT TESTMODE EQUAL 1)
	FILE(GLOB_RECURSE testFiles ${PROJECT_HOME_DIRECTORY}/source/*.Test.cpp)
	list(REMOVE_ITEM sourceFiles ${testFiles})
ENDIF ()

# Get a list of our third party libraries
#${THIRD_PARTY_LIBRARY_PATH}
IF (RELEASE) 
  SET(THIRD_PARTY_LIB_PATH "${THIRD_PARTY_PATH}/lib/release")
ELSE (RELEASE)
	SET(THIRD_PARTY_LIB_PATH "${THIRD_PARTY_PATH}/lib/debug")
ENDIF ()
MESSAGE("Third Party Library Path: ${THIRD_PARTY_LIB_PATH}")
LINK_DIRECTORIES("${THIRD_PARTY_LIB_PATH}")
LINK_DIRECTORIES("${THIRD_PARTY_PATH}/lib/special/")

IF (MSVC)
	FILE(GLOB thirdPartyLibrariesRaw RELATIVE ${thirdPartyFolderAbs} ${thirdPartyFolder}*.lib)
	FOREACH(library ${thirdPartyLibrariesRaw})
  		STRING(REPLACE ${thirdPartyFolderAbs} "" shortLibrary ${library})
  		SET(thirdPartyLibraries " ")
	ENDFOREACH()
	SET(thirdPartyLibraries " ")
ELSE(MSVC)
	MESSAGE("-- Loading third party libraries from ${THIRD_PARTY_LIB_PATH}")
	FILE(GLOB THIRD_PARTY_LIBRARIES ${THIRD_PARTY_LIB_PATH}/*.a)
	FOREACH(library ${THIRD_PARTY_LIBRARIES})
	  MESSAGE("-- Found library: ${library}")
	  STRING(REPLACE "${THIRD_PARTY_LIB_PATH}/" "" shortLibrary ${library})
		MESSAGE("-- Short Version: ${shortLibrary}")
	  SET(thirdPartyLibraries ${thirdPartyLibraries} -l:${shortLibrary})
	ENDFOREACH()
ENDIF()

# Load our Special Libraries as well
FOREACH(library ${specialLibraries})
	SET(thirdPartyLibraries ${thirdPartyLibraries} ${library})
ENDFOREACH()

# We need to sort library order so gtest and gmock go first
IF (TESTMODE EQUAL 1)
  LIST(SORT thirdPartyLibraries)
ENDIF()

SET(LINK_OPTIONS " ")
IF (LIBRARY)
  ADD_LIBRARY(${EXE_NAME} SHARED ${sourceFiles} )
  IF(WIN32)
    SET(LINK_OPTIONS "-static")
  ENDIF(WIN32)
ELSE(LIBRARY)
	IF(NOT TESTMODE)
		IF (NOT MSVC)
			IF(WIN32)
			MESSAGE("Building ICON")
			ADD_CUSTOM_TARGET(icon ALL COMMAND windres --output "${BUILD_SYSTEM_DIRECTORY}/icon.o" --input "${BUILD_SYSTEM_DIRECTORY}/casal2.rc")
			MESSAGE("Icon Built")
			#SET(LINK_OPTIONS "-static-libgcc -static-libstdc++ -static ${CMAKE_HOME_DIRECTORY}/../BuildSystem/icon.o")
			SET(LINK_OPTIONS "-static ${BUILD_SYSTEM_DIRECTORY}/icon.o")
			ENDIF(WIN32)
		ELSE()
			SET(LINK_OPTIONS " ")
		ENDIF()
	ENDIF()

	MESSAGE("-- Link Options: ${LINK_OPTIONS}")
	MESSAGE("-- Third party libraries: ${thirdPartyLibraries}")
	ADD_EXECUTABLE(${EXE_NAME} ${sourceFiles} )
ENDIF(LIBRARY)


SET_TARGET_PROPERTIES(${EXE_NAME} PROPERTIES COMPILE_FLAGS ${COMPILE_OPTIONS} LINK_FLAGS ${LINK_OPTIONS})
TARGET_LINK_LIBRARIES(${EXE_NAME} ${thirdPartyLibraries})
IF (NOT LIBRARY)
	IF(NOT TESTMODE)
		IF (NOT MSVC)
			IF(WIN32)
			ADD_DEPENDENCIES(${EXE_NAME} icon)
			ENDIF(WIN32)
		ENDIF()
	ENDIF()
ENDIF()
