@echo off
rem expects Casal2 to be built recently with doBuild archive
cd Simple
..\..\BuildSystem\Casal2\casal2 -E MPD.dat -o free.dat > estimate.log 2> estimate.err
..\..\BuildSystem\Casal2\casal2 -r -i free.dat > run.log 2> run.err
..\..\BuildSystem\Casal2\casal2 -r -i free.dat -t > run.tab 2> run.tab.err
..\..\BuildSystem\Casal2\casal2 -p -i free.dat -o profile.dat > profile.log 2> profile.err
rm *.1
..\..\BuildSystem\Casal2\casal2 -M MPD.dat -g 0 > MCMC.log 2> MCMC.err

cd ..\2stock
..\..\BuildSystem\Casal2\casal2 -E MPD.dat -o free.dat > estimate.log 2> estimate.err
..\..\BuildSystem\Casal2\casal2 -r -i free.dat > run.log 2> run.err
..\..\BuildSystem\Casal2\casal2 -r -i free.dat -t > run.tab 2> run.tab.err
rem ..\..\BuildSystem\Casal2\casal2 -p -i free.dat -o profile.dat > profile.log 2> profile.err
rm *.1
..\..\BuildSystem\Casal2\casal2 -M MPD.dat -g 0 > MCMC.log 2> MCMC.err

cd ..\LengthBasedModel
..\..\BuildSystem\Casal2\casal2 -E MPD.dat -o free.dat > estimate.log 2> estimate.err
..\..\BuildSystem\Casal2\casal2 -r -i free.dat > run.log 2> run.err
..\..\BuildSystem\Casal2\casal2 -r -i free.dat -t > run.tab 2> run.tab.err
rem ..\..\BuildSystem\Casal2\casal2 -p -i free.dat -o profile.dat > profile.log 2> profile.err
rm *.1
..\..\BuildSystem\Casal2\casal2 -M MPD.dat -g 0 > MCMC.log 2> MCMC.err

cd ..