#!/bin/sh

cd Simple
../../BuildSystem/Casal2/casal2 -E MPD.dat -o free.dat > estimate.log >& estimate.err
../../BuildSystem/Casal2/casal2 -r -i free.dat > run.log >& run.err
../../BuildSystem/Casal2/casal2 -p -i free.dat -o profile.dat > profile.log >& profile.err
../../BuildSystem/Casal2/casal2 -M MPD.dat -g 0 > MCMC.log >& MCMC.err

cd ../2stock
../../BuildSystem/Casal2/casal2 -E MPD.dat -o free.dat > estimate.log >& estimate.err
../../BuildSystem/Casal2/casal2 -r -i free.dat > run.log >& run.err
../../BuildSystem/Casal2/casal2 -p -i free.dat -o profile.dat > profile.log >& profile.err
../../BuildSystem/Casal2/casal2 -M MPD.dat -g 0 > MCMC.log >& MCMC.err

cd ..
