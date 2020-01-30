@echo off
rem this assumes that 'release adolc', 'release betadiff', and 'release cppad' have already been built successfully
rem this script runs in directory Examples

cd Simple
rem del params_est.out
..\..\..\..\..\BuildSystem\bin\windows\release_betadiff\casal2 -e -o params_est.out > run_estimation.txt 2>&1

cd ..\2stock
rem del params_est.out
..\..\..\..\..\BuildSystem\bin\windows\release_betadiff\casal2 -e -o params_est.out > run_estimation.txt 2>&1

