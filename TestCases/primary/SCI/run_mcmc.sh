# this assumes that the estimation runs have completed successfully (i.e., params_est.out exists for the Casal2 models)
# start in directory TestCases/primary/[assessment]

cd CASAL
rm -f params_mcmc.out objectives.* samples.*
../../../CASAL/casal -m -o params_mcmc.out -g 850088731 > run_mcmc.txt 2>&1 &
