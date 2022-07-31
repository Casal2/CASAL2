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
..\..\..\..\BuildSystem\Casal2\casal2 -E mpd.out -c betadiff_config.csl2 -o betadiff_params_est.out > betadiff.log 2> betadiff.err

rem del betadiff_low_tolerance_params_est.out
..\..\..\..\BuildSystem\Casal2\casal2 -E mpd.out -c betadiff_low_tolerance_config.csl2 -o betadiff_low_tolerance_params_est.out > betadiff_low_tolerance.log 2> betadiff_low_tolerance.err

rem del adolc_params_est.out
..\..\..\..\BuildSystem\Casal2\casal2 -E mpd.out -c adolc_config.csl2 -o adolc_params_est.out > adolc.log 2> adolc.err

rem del adolc_low_tolerance_params_est.out
..\..\..\..\BuildSystem\Casal2\casal2 -E mpd.out -c adolc_low_tolerance_config.csl2 -o adolc_low_tolerance_params_est.out > adolc_low_tolerance.log 2> adolc_low_tolerance.err

