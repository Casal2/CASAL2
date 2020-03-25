# this assumes that 'release adolc', 'release betadiff', and 'release cppad' have already been built successfully
# and that the run_models.sh script has been run already
# start in directory TestCases/primary/HOK

cd CASAL
../../../casal -m > run_mcmc.txt 2>&1 &

cd ../CASAL_sens1
../../../casal -m > run_mcmc.txt 2>&1 &


cd ../Casal2/betadiff_casal_flags_off
../../../../../BuildSystem/bin/linux/release_betadiff/casal2 -m --skip-estimation > run_mcmc.txt 2>&1 &

cd ../betadiff_casal_flags_on
../../../../../BuildSystem/bin/linux/release_betadiff/casal2 -m --skip-estimation > run_mcmc.txt 2>&1 &

cd ../betadiff_casal_flags_on_low_tol
../../../../../BuildSystem/bin/linux/release_betadiff/casal2 -m --skip-estimation > run_mcmc.txt 2>&1 &


cd ../cppad_casal_flags_off
../../../../../BuildSystem/bin/linux/release_cppad/casal2 -m --skip-estimation > run_mcmc.txt 2>&1 &

cd ../cppad_casal_flags_on
../../../../../BuildSystem/bin/linux/release_cppad/casal2 -m --skip-estimation > run_mcmc.txt 2>&1 &


cd ../adolc_casal_flags_off
../../../../../BuildSystem/bin/linux/release_adolc/casal2 -m --skip-estimation > run_mcmc.txt 2>&1 &

cd ../adolc_casal_flags_on
../../../../../BuildSystem/bin/linux/release_adolc/casal2 -m --skip-estimation > run_mcmc.txt 2>&1 &

cd ../adolc_casal_flags_on_low_tol
../../../../../BuildSystem/bin/linux/release_adolc/casal2 -m --skip-estimation > run_mcmc.txt 2>&1 &

