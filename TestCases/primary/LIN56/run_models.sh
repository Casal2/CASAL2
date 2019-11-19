# this assumes that 'release adolc', 'release betadiff', and 'release cppad' have already been built successfully
# start in directory TestCases/primary/LIN56

cd CASAL
../../casal -e -o params_est.out > run_estimation.txt 2>&1

cd ../CASAL_sens1
../../casal -e -o params_est.out > run_estimation.txt 2>&1



cd ../Casal2/adolc
../../../../../BuildSystem/bin/linux/release_adolc/casal2 -e -o params_est.out > run_estimation.txt 2>&1

cd ../casal_flag_on
../../../../../BuildSystem/bin/linux/release_betadiff/casal2 -e -o params_est.out > run_estimation.txt 2>&1

cd ../casal_flag_on_low_tol
../../../../../BuildSystem/bin/linux/release_betadiff/casal2 -e -o params_est.out > run_estimation.txt 2>&1

cd ../casal_flag_off
../../../../../BuildSystem/bin/linux/release_betadiff/casal2 -e -o params_est.out > run_estimation.txt 2>&1

cd ../cppad
../../../../../BuildSystem/bin/linux/release_cppad/casal2 -e -o params_est.out > run_estimation.txt 2>&1

