
cd Simple
..\..\BuildSystem\bin\linux_gcc\release\Casal2 -E MPD.dat -o free.dat > estimate.log >& estimate.err 
..\..\BuildSystem\bin\linux_gcc\release\Casal2 -r -i free.dat > run.log >& run.err
..\..\BuildSystem\bin\linux_gcc\release\Casal2 -p -i free.dat -o profile.dat > profile.log >& profile.err
..\..\BuildSystem\bin\linux_gcc\release\Casal2 -M MPD.dat -g 0 > MCMC.log >& MCMC.err

cd ..\2stock
..\..\BuildSystem\bin\linux_gcc\release_betadiff\Casal2 -E MPD.dat -o free.dat > estimate.log >& estimate.err
..\..\BuildSystem\bin\linux_gcc\release\Casal2 -r -i free.dat > run.log >& run.err
..\..\BuildSystem\bin\linux_gcc\release_betadiff\Casal2 -p -i free.dat -o profile.dat > profile.log >& profile.err
..\..\BuildSystem\bin\linux_gcc\release\Casal2 -M MPD.dat -g 0 > MCMC.log >& MCMC.err

cd ..