@echo off

cd Simple
..\..\BuildSystem\bin\windows_gcc\release\Casal2 -E MPD.dat -o free.dat > estimate.log 2> estimate.err
..\..\BuildSystem\bin\windows_gcc\release\Casal2 -r -i free.dat > run.log 2> run.err
..\..\BuildSystem\bin\windows_gcc\release\Casal2 -p -i free.dat -o profile.dat > profile.log 2> profile.err
..\..\BuildSystem\bin\windows_gcc\release\Casal2 -M MPD.dat -g 0 > MCMC.log 2> MCMC.err

cd ..\2stock
..\..\BuildSystem\bin\windows_gcc\release_betadiff\Casal2 -E MPD.dat -o free.dat > estimate.log 2> estimate.err
..\..\BuildSystem\bin\windows_gcc\release\Casal2 -r -i free.dat > run.log 2> run.err
..\..\BuildSystem\bin\windows_gcc\release_betadiff\Casal2 -p -i free.dat -o profile.dat > profile.log 2> profile.err
..\..\BuildSystem\bin\windows_gcc\release\Casal2 -M MPD.dat -g 0 > MCMC.log 2> MCMC.err

cd ..