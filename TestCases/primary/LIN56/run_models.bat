@echo off
rem this assumes that 'release adolc', 'release betadiff', and 'release cppad' have already been built successfully
rem this script runs in directory TestCases\primary\LIN56

cd CASAL
..\..\casal.exe -e -o params_est.out > run_estimation.txt 2>&1

cd ..\CASAL_sens1
..\..\casal.exe -e -o params_est.out > run_estimation.txt 2>&1


cd ..\Casal2\adolc_casal_flags_off
..\..\..\..\..\BuildSystem\bin\windows\release_adolc\casal2 -e -o params_est.out > run_estimation.txt 2>&1

cd ..\adolc_casal_flags_on
..\..\..\..\..\BuildSystem\bin\windows\release_adolc\casal2 -e -o params_est.out > run_estimation.txt 2>&1

cd ..\adolc_casal_flags_on_low_tol
..\..\..\..\..\BuildSystem\bin\windows\release_adolc\casal2 -e -o params_est.out > run_estimation.txt 2>&1


cd ..\betadiff_casal_flags_off
..\..\..\..\..\BuildSystem\bin\windows\release_betadiff\casal2 -e -o params_est.out > run_estimation.txt 2>&1

cd ..\betadiff_casal_flags_on
..\..\..\..\..\BuildSystem\bin\windows\release_betadiff\casal2 -e -o params_est.out > run_estimation.txt 2>&1

cd ..\betadiff_casal_flags_on_low_tol
..\..\..\..\..\BuildSystem\bin\windows\release_betadiff\casal2 -e -o params_est.out > run_estimation.txt 2>&1


cd ..\cppad_casal_flags_off
..\..\..\..\..\BuildSystem\bin\windows\release_cppad\casal2 -e -o params_est.out > run_estimation.txt 2>&1

cd ..\cppad_casal_flags_on
..\..\..\..\..\BuildSystem\bin\windows\release_cppad\casal2 -e -o params_est.out > run_estimation.txt 2>&1

