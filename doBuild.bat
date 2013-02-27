@echo off

set oldPath=%PATH%

PATH=%PATH%;

%CD%\buildtools\windows\python27\python build.py %1 %2 %3 %4 %5 %6 %7 %8 %9

PATH=%oldPath%

exit /b %ERRORLEVEL%
