# this assumes that 'release betadiff' and 'release cppad' have already been built successfully
# in directory TestCases/primary/LIN56

cd CASAL
../../casal -e -o params_est.out > run_estimation.txt 2>&1

cd ../Casal2/casal_flag_on
../../../../../BuildSystem/bin/linux/release_betadiff/casal2 -e -o params_est.out > run_estimation.txt 2>&1

cd ../casal_flag_off
../../../../../BuildSystem/bin/linux/release_betadiff/casal2 -e -o params_est.out > run_estimation.txt 2>&1

cd ../cppad
../../../../../BuildSystem/bin/linux/release_cppad/casal2 -e -o params_est.out > run_estimation.txt 2>&1

