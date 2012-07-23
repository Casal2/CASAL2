@echo off

set oldPath=%PATH%

PATH=%PATH%;%CD%\buildtools\windows\python27;

python build.py

PATH=%oldPath%
