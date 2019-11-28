# this assumes that 'release adolc', 'release betadiff', and 'release cppad' have already been built successfully
# start in directory TestCases/primary/LIN56

cd CASAL
../../casal -e -o params_est.out > run_estimation.txt 2>&1

cd ../CASAL_sens1
../../casal -e -o params_est.out > run_estimation.txt 2>&1



cd ../Casal2/adolc_casal_flags_off
../../../../../BuildSystem/bin/linux/release_adolc/casal2 -e -o params_est.out > run_estimation.txt 2>&1

cd ../adolc_casal_flags_on
../../../../../BuildSystem/bin/linux/release_adolc/casal2 -e -o params_est.out > run_estimation.txt 2>&1

cd ../adolc_casal_flags_on_low_tol
../../../../../BuildSystem/bin/linux/release_adolc/casal2 -e -o params_est.out > run_estimation.txt 2>&1


cd ../betadiff_casal_flags_off
../../../../../BuildSystem/bin/linux/release_betadiff/casal2 -e -o params_est.out > run_estimation.txt 2>&1

cd ../betadiff_casal_flags_on
../../../../../BuildSystem/bin/linux/release_betadiff/casal2 -e -o params_est.out > run_estimation.txt 2>&1

cd ../betadiff_casal_flags_on_low_tol
../../../../../BuildSystem/bin/linux/release_betadiff/casal2 -e -o params_est.out > run_estimation.txt 2>&1


cd ../cppad_casal_flags_off
../../../../../BuildSystem/bin/linux/release_cppad/casal2 -e -o params_est.out > run_estimation.txt 2>&1

cd ../cppad_casal_flags_on
../../../../../BuildSystem/bin/linux/release_cppad/casal2 -e -o params_est.out > run_estimation.txt 2>&1

