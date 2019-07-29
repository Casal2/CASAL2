# - Find tclsh
# This module finds if TCL is installed and determines where the
# include files and libraries are. It also determines what the name of
# the library is. This code sets the following variables:
#  TCLSH_FOUND = TRUE if tclsh has been found
#  TCL_TCLSH = the path to the tclsh executable
# In cygwin, look for the cygwin version first.  Don't look for it later to
# avoid finding the cygwin version on a Win32 build.

#=============================================================================
# Copyright 2001-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

IF(CYGWIN)
  FIND_PROGRAM(TCL_TCLSH NAMES cygtclsh83 cygtclsh80)
ENDIF(CYGWIN)

GET_FILENAME_COMPONENT(TK_WISH_PATH "${TK_WISH}" PATH)
GET_FILENAME_COMPONENT(TK_WISH_PATH_PARENT "${TK_WISH_PATH}" PATH)
STRING(REGEX REPLACE 
  "^.*wish([0-9]\\.*[0-9]).*$" "\\1" TK_WISH_VERSION "${TK_WISH}")

GET_FILENAME_COMPONENT(TCL_INCLUDE_PATH_PARENT "${TCL_INCLUDE_PATH}" PATH)
GET_FILENAME_COMPONENT(TK_INCLUDE_PATH_PARENT "${TK_INCLUDE_PATH}" PATH)

GET_FILENAME_COMPONENT(TCL_LIBRARY_PATH "${TCL_LIBRARY}" PATH)
GET_FILENAME_COMPONENT(TCL_LIBRARY_PATH_PARENT "${TCL_LIBRARY_PATH}" PATH)
STRING(REGEX REPLACE 
  "^.*tcl([0-9]\\.*[0-9]).*$" "\\1" TCL_LIBRARY_VERSION "${TCL_LIBRARY}")

GET_FILENAME_COMPONENT(TK_LIBRARY_PATH "${TK_LIBRARY}" PATH)
GET_FILENAME_COMPONENT(TK_LIBRARY_PATH_PARENT "${TK_LIBRARY_PATH}" PATH)
STRING(REGEX REPLACE 
  "^.*tk([0-9]\\.*[0-9]).*$" "\\1" TK_LIBRARY_VERSION "${TK_LIBRARY}")

SET(TCLTK_POSSIBLE_BIN_PATHS
  "${TCL_INCLUDE_PATH_PARENT}/bin"
  "${TK_INCLUDE_PATH_PARENT}/bin"
  "${TCL_LIBRARY_PATH_PARENT}/bin"
  "${TK_LIBRARY_PATH_PARENT}/bin"
  "${TK_WISH_PATH_PARENT}/bin"
  )

IF(WIN32)
  GET_FILENAME_COMPONENT(
    ActiveTcl_CurrentVersion 
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\ActiveState\\ActiveTcl;CurrentVersion]" 
    NAME)
  SET(TCLTK_POSSIBLE_BIN_PATHS ${TCLTK_POSSIBLE_BIN_PATHS}
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\ActiveState\\ActiveTcl\\${ActiveTcl_CurrentVersion}]/bin"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Scriptics\\Tcl\\8.6;Root]/bin"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Scriptics\\Tcl\\8.5;Root]/bin"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Scriptics\\Tcl\\8.4;Root]/bin"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Scriptics\\Tcl\\8.3;Root]/bin"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Scriptics\\Tcl\\8.2;Root]/bin"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Scriptics\\Tcl\\8.0;Root]/bin"
    )
ENDIF(WIN32)

SET(TCL_TCLSH_NAMES
  tclsh
  tclsh${TCL_LIBRARY_VERSION} tclsh${TK_LIBRARY_VERSION} tclsh${TK_WISH_VERSION}
  tclsh86 tclsh8.6
  tclsh85 tclsh8.5
  tclsh84 tclsh8.4
  tclsh83 tclsh8.3
  tclsh82 tclsh8.2
  tclsh80 tclsh8.0
  )

FIND_PROGRAM(TCL_TCLSH
  NAMES ${TCL_TCLSH_NAMES}
  HINTS ${TCLTK_POSSIBLE_BIN_PATHS}
  )

IF(TCL_TCLSH)
   EXECUTE_PROCESS(COMMAND "${CMAKE_COMMAND}" -E echo puts "\$tcl_version"
                   COMMAND "${TCL_TCLSH}"
                   OUTPUT_VARIABLE TCLSH_VERSION_STRING
                   ERROR_QUIET
                   OUTPUT_STRIP_TRAILING_WHITESPACE)
ENDIF(TCL_TCLSH)

# handle the QUIETLY and REQUIRED arguments and set TIFF_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Tclsh
                                  REQUIRED_VARS TCL_TCLSH
                                  VERSION_VAR TCLSH_VERSION_STRING)

MARK_AS_ADVANCED(TCL_TCLSH)
