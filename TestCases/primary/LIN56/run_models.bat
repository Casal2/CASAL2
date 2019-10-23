@echo off
rem this assumes that 'release betadiff' and 'release cppad' have already been built successfully
:: Assumes casal.exe is in your system path
:: in directory TestCases\primary\LIN56

cd CASAL
casal -e -o params_est.out > run_estimation.txt 2>&1

cd ..\Casal2\casal_flags_on
..\..\..\..\..\BuildSystem\bin\windows\release_betadiff\casal2 -e -o params_est.out > run_estimation.txt 2>&1

cd ..\casal_flags_off
..\..\..\..\..\BuildSystem\bin\windows\release_betadiff\casal2 -e -o params_est.out > run_estimation.txt 2>&1

cd ..\cppad
..\..\..\..\..\BuildSystem\bin\windows\release_cppad\casal2 -e -o params_est.out > run_estimation.txt 2>&1

