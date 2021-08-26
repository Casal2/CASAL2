cd Simple
..\..\BuildSystem\bin\windows_gcc\release\Casal2 -E MPD.dat -o params.dat > estimate.log 2>&1 estimate.err
..\..\BuildSystem\bin\windows_gcc\release\Casal2 -p -i params.dat -o profile.dat > profile.log 2>&1 profile.err
..\..\BuildSystem\bin\windows_gcc\release\Casal2 -M MPD.dat -g 0 > MCMC.log 2>&1 MCMC.err

cd ..\2stock
..\..\BuildSystem\bin\windows_gcc\release\Casal2 -E MPD.dat -o params.dat > estimate.log 2>&1 estimate.err
..\..\BuildSystem\bin\windows_gcc\release\Casal2 -p -i params.dat -o profile.dat > profile.log 2>&1 profile.err
..\..\BuildSystem\bin\windows_gcc\release\Casal2 -M MPD.dat -g 0 > MCMC.log 2>&1 MCMC.err

cd ..