# this assumes that 'release adolc' and 'release betadiff' have already been built successfully
# start in directory TestCases/primary/SBW

# cd CASAL
# rm -f params_est.out
# ../../../CASAL/casal -e -o params_est.out > run_estimation.txt 2>&1

# cd ../CASAL_sens1
# rm -f params_est.out
# ../../../CASAL/casal -e -o params_est.out > run_estimation.txt 2>&1


cd Casal2/betadiff_casal_flags_off
rm -f params_est.out
../../../../../BuildSystem/bin/linux_gcc/release_betadiff/casal2 -e -o params_est.out > run_estimation.txt 2>&1

cd ../betadiff_casal_flags_on
rm -f params_est.out
../../../../../BuildSystem/bin/linux_gcc/release_betadiff/casal2 -e -o params_est.out > run_estimation.txt 2>&1

cd ../betadiff_casal_flags_on_low_tol
rm -f params_est.out
../../../../../BuildSystem/bin/linux_gcc/release_betadiff/casal2 -e -o params_est.out > run_estimation.txt 2>&1


cd ../adolc_casal_flags_off
rm -f params_est.out
../../../../../BuildSystem/bin/linux_gcc/release_adolc/casal2 -e -o params_est.out > run_estimation.txt 2>&1

cd ../adolc_casal_flags_on
rm -f params_est.out
../../../../../BuildSystem/bin/linux_gcc/release_adolc/casal2 -e -o params_est.out > run_estimation.txt 2>&1

cd ../adolc_casal_flags_on_low_tol
rm -f params_est.out
../../../../../BuildSystem/bin/linux_gcc/release_adolc/casal2 -e -o params_est.out > run_estimation.txt 2>&1

