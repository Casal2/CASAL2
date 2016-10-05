cmake_minimum_required(VERSION 2.8)
project(UbuntuThreading)

# Variables
SET(EXE_NAME "test_library")
SET(LIBRARY_NAME "shared_library")

# Generic Compile Options
SET(COMPILE_OPTIONS "-O2 -g0 -Wall -Werror -fmessage-length=0 -Wno-unused-local-typedefs")

################################################################
# COMPILE OPTIONS FOR DIFFERENT LINUX DISTRIBUTIONS
################################################################

# 1. Works under OpenSuse. Segfault on Ubuntu when creating thread
# SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -fPIC -std=c++11")

# 2. Segfauls on Ubuntu when creating thread
SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -pthread -fPIC -std=c++11")
#SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -fPIC -std=c++11 -pthread")
#SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -fPIC -std=c++11 -static")
#SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -fPIC -std=c++11 -static -pthread")
#SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -fPIC -std=c++11 -Wl,--whole-archive -static -pthread")
#SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -fPIC -std=c++11 -Wl,--whole-archive -pthread")
#SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -fPIC -std=c++11 -Wl,-M -pthread")
#SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -fPIC -std=c++11 -Wl,--whole-archive -lpthread -Wl,--no-whole-archive")

# 3. Ubuntu: std::system_error with
#             Enable multithreading to use std::thread: Operation not permitted
#SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -fPIC -std=c++11 -pthread -D_GLIBCXX_GTHREAD_USE_WEAK=0")
#SET(COMPILE_OPTIONS "${COMPILE_OPTIONS} -fPIC -std=c++11 -pthread -D_GLIBCXX_GTHREAD_USE_WEAK=0 -Wl,-M")

#################################################################
# LINKING OPTIONS
#################################################################
SET(EXE_LIBS dl rt)
SET(LIB_LIBS pthread)

# KEY TO SUCCESS: If we don't compile the exe with -pthread here then it'll never work on Ubuntu
SET(EXE_LINK_OPTIONS "-pthread")
SET(LIB_LINK_OPTIONS " ")

MESSAGE("Compiling with: ${COMPILE_OPTIONS}")
ADD_EXECUTABLE(${EXE_NAME} "Main.cpp" )
SET_TARGET_PROPERTIES(${EXE_NAME} PROPERTIES COMPILE_FLAGS ${COMPILE_OPTIONS} LINK_FLAGS ${EXE_LINK_OPTIONS})
TARGET_LINK_LIBRARIES(${EXE_NAME} ${EXE_LIBS})

ADD_LIBRARY(${LIBRARY_NAME} SHARED "SharedLibrary.cpp" )
SET_TARGET_PROPERTIES(${LIBRARY_NAME} PROPERTIES COMPILE_FLAGS ${COMPILE_OPTIONS} LINK_FLAGS ${LIB_LINK_OPTIONS})
TARGET_LINK_LIBRARIES(${LIBRARY_NAME} ${LIB_LIBS})
