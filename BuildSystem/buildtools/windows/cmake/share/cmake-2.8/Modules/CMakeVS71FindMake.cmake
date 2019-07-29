
#=============================================================================
# Copyright 2003-2009 Kitware, Inc.
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

FIND_PROGRAM(CMAKE_MAKE_PROGRAM
  NAMES devenv
  PATHS
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\7.1\\Setup\\VS;EnvironmentDirectory]
  "$ENV{ProgramFiles}/Microsoft Visual Studio .NET/Common7/IDE"
  "c:/Program Files/Microsoft Visual Studio .NET/Common7/IDE"
  "c:/Program Files/Microsoft Visual Studio.NET/Common7/IDE"
  "/Program Files/Microsoft Visual Studio .NET/Common7/IDE/"
  )
MARK_AS_ADVANCED(CMAKE_MAKE_PROGRAM)
SET(MSVC71 1)
SET(MSVC_VERSION 1310)
