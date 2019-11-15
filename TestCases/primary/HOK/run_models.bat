@echo off
rem this assumes that 'release betadiff' has already been built successfully
:: Assumes casal.exe is in your system path
:: in directory TestCases\primary\HOK

cd CASAL
casal -r -i start_pars.txt > check_start.log
casal -r -i params_est.out > run.log
casal -e -i params_est.out > estimate.log 2> log.out

cd ..\Casal2
..\..\..\..\..\BuildSystem\bin\windows\release_betadiff\casal2 -r -i start_pars.txt > check_start.log
..\..\..\..\..\BuildSystem\bin\windows\release_betadiff\casal2 -r -i params_est.out > run.log
..\..\..\..\..\BuildSystem\bin\windows\release_betadiff\casal2 -e -i params_est.out > estimate.log 2> log.out
