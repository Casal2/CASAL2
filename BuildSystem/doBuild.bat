@echo off

set oldPath=%PATH%

PATH=%PATH%;%CD%\buildtools\windows\unixutils\

IF EXIST %CD%\buildtools\windows\python27 (echo Skipping Python Decompression) ELSE (echo Decompressing Python && unzip -qq buildtools\windows\python27.zip -d buildtools\windows\ && echo Python27 Decompressed)

%CD%\buildtools\windows\python27\python build.py %1 %2 %3 %4 %5 %6 %7 %8 %9

PATH=%oldPath%

exit /b %ERRORLEVEL%
