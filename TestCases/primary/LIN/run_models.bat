@echo off
rem this assumes that these scripts have all been run
rem ./doBuild.bat thirdparty ; ./doBuild.bat library release ; ./doBuild.bat library adolc ; ./doBuild.bat library betadiff ; ./doBuild.bat library test ; ./doBuild.bat frontend ; ./doBuild.bat archive true
rem this script runs in directory TestCases\primary\LIN

rem cd CASAL
rem del params_est.out
rem ..\..\..\CASAL\casal.exe -e -o params_est.out > run_estimation.txt 2>&1

rem cd ..\CASAL_sens1
rem del params_est.out
rem ..\..\..\CASAL\casal.exe -e -o params_est.out > run_estimation.txt 2>&1


cd Casal2\betadiff_casal_flags_off
del params_est.out
..\..\..\..\..\BuildSystem\Casal2\casal2 -e -o params_est.out > run_estimation.txt 2>&1

cd ..\betadiff_casal_flags_on
del params_est.out
..\..\..\..\..\BuildSystem\Casal2\casal2 -e -o params_est.out > run_estimation.txt 2>&1

cd ..\betadiff_casal_flags_on_low_tol
del params_est.out
..\..\..\..\..\BuildSystem\Casal2\casal2 -e -o params_est.out > run_estimation.txt 2>&1


cd ..\adolc_casal_flags_off
del params_est.out
..\..\..\..\..\BuildSystem\Casal2\casal2 -e -o params_est.out > run_estimation.txt 2>&1

cd ..\adolc_casal_flags_on
del params_est.out
..\..\..\..\..\BuildSystem\Casal2\casal2 -e -o params_est.out > run_estimation.txt 2>&1

cd ..\adolc_casal_flags_on_low_tol
del params_est.out
..\..\..\..\..\BuildSystem\Casal2\casal2 -e -o params_est.out > run_estimation.txt 2>&1

