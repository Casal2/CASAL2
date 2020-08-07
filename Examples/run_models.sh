# this assumes that 'release adolc', 'release betadiff', and 'release cppad' have already been built successfully
# start in directory Examples

cd Simple
# rm -f params_est.out
../../BuildSystem/bin/linux/release_betadiff/casal2 -e -o params_est.out > run_estimation.txt 2>&1

cd ../2stock
# rm -f params_est.out
../../BuildSystem/bin/linux/release_betadiff/casal2 -e -o params_est.out > run_estimation.txt 2>&1

