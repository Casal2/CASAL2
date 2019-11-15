# this assumes that 'release adolc', 'release betadiff', have already been built successfully
# start in directory TestCases/primary/HOK

cd CASAL
../../casal -r -i start_pars.txt > check_start.log
../../casal -r -i params_est.out > run.log
../../casal -e -i params_est.out > estimate.log 2> log.out

cd ../Casal2/adolc
../../../../../BuildSystem/bin/linux/release_adolc/casal2 -r -i start_pars.txt > check_start.log
../../../../../BuildSystem/bin/linux/release_adolc/casal2 -r -i params_est.out > run.log
../../../../../BuildSystem/bin/linux/release_adolc/casal2 -e -i params_est.out > estimate.log 2> log.out
