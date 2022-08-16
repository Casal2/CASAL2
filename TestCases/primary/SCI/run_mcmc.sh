# this assumes that the estimation runs have completed successfully (i.e., params_est.out exists for the Casal2 models)
# start in directory TestCases/primary/[assessment]

cd CASAL
rm -f params_mcmc.out objectives.* samples.*
../../../CASAL/casal -m -o params_mcmc.out -g 850088731 > run_mcmc.txt 2>&1 &

cd ../CASAL_sens1
rm -f params_mcmc.out objectives.* samples.*
../../../CASAL/casal -m -o params_mcmc.out -g 850088731 > run_mcmc.txt 2>&1 &


cd ../Casal2/betadiff_casal_flags_off
rm -f params_mcmc.out mcmc*.out objectives.* samples.*
../../../../../BuildSystem/Casal2/casal2 -M mpd.out -g 850088731 > run_mcmc.txt 2>&1 &

cd ../betadiff_casal_flags_on
rm -f params_mcmc.out mcmc*.out objectives.* samples.*
../../../../../BuildSystem/Casal2/casal2 -M mpd.out -g 850088731 > run_mcmc.txt 2>&1 &

cd ../betadiff_casal_flags_on_low_tol
rm -f params_mcmc.out mcmc*.out objectives.* samples.*
../../../../../BuildSystem/Casal2/casal2 -M mpd.out -g 850088731 > run_mcmc.txt 2>&1 &


cd ../adolc_casal_flags_off
rm -f params_mcmc.out mcmc*.out objectives.* samples.*
../../../../../BuildSystem/Casal2/casal2 -M mpd.out -g 850088731 > run_mcmc.txt 2>&1 &

cd ../adolc_casal_flags_on
rm -f params_mcmc.out mcmc*.out objectives.* samples.*
../../../../../BuildSystem/Casal2/casal2 -M mpd.out -g 850088731 > run_mcmc.txt 2>&1 &

cd ../adolc_casal_flags_on_low_tol
rm -f params_mcmc.out mcmc*.out objectives.* samples.*
../../../../../BuildSystem/Casal2/casal2 -M mpd.out -g 850088731 > run_mcmc.txt 2>&1 &

