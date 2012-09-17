@echo off

set oldPath=%PATH%

PATH=%CD%\buildtools\windows\python27;%PATH%;

python build.py %1 %2 %3 %4 %5 %6 %7 %8 %9

PATH=%oldPath%
