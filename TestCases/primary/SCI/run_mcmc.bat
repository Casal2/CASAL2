@echo off
rem this assumes that these scripts have all been run
rem ./doBuild.bat archive
rem this script runs in directory TestCases\primary\SBW

rem cd CASAL
rem del params_est.out
rem ..\..\..\CASAL\casal.exe -q -e -o params_est.out > estimation.log 2> estimation.err

rem cd ..\CASAL_sens1
rem del params_est.out
rem ..\..\..\CASAL\casal.exe -q -e -o params_est.out > run_estimation.txt 2> estimation.err

cd Casal2
rem del betadiff_params_est.out
..\..\..\..\BuildSystem\Casal2\casal2 -m  -c betadiff_config.csl2  > mcmc.log 2> mcmc.err

