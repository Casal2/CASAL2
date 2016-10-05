cmake_minimum_required(VERSION 2.8)
project(UbuntuThreading)

# Variables
SET(EXE_NAME "test_library")
SET(LIBRARY_NAME "shared_library")

# Change the output folders for our build to something cleaner and platform dependent
SET(CMAKE_ROOT   bin/cmake)


SET(COMPILE_OPTIONS "-O2 -g0 -Wall -Werror -fmessage-length=0 -Wno-unused-local-typedefs")
SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -std=c++11")
SET(LIBS dl pthread rt)

# BUILD PARAMETERS
MESSAGE("COMPILE_OPTIONS: ${COMPILE_OPTIONS}")

INCLUDE_DIRECTORIES(SYSTEM "${CMAKE_HOME_DIRECTORY}../BuildSystem/bin/${buildOS}/thirdparty/include")

SET(LINK_OPTIONS " ")

ADD_EXECUTABLE(${EXE_NAME} "Main.cpp" )
SET_TARGET_PROPERTIES(${EXE_NAME} PROPERTIES COMPILE_FLAGS ${COMPILE_OPTIONS} LINK_FLAGS ${LINK_OPTIONS})
TARGET_LINK_LIBRARIES(${EXE_NAME} ${LIBS})

ADD_LIBRARY(${LIBRARY_NAME} SHARED "SharedLibrary.cpp" )
SET_TARGET_PROPERTIES(${LIBRARY_NAME} PROPERTIES COMPILE_FLAGS ${COMPILE_OPTIONS} LINK_FLAGS ${LINK_OPTIONS})