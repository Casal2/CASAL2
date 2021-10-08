# this assumes that these scripts have all been run
# ./doBuild.sh thirdparty ; ./doBuild.sh library release ; ./doBuild.sh library adolc ; ./doBuild.sh library betadiff ; ./doBuild.sh library test ; ./doBuild.sh frontend ; ./doBuild.sh archive true
# start in directory TestCases/primary/LIN

# cd CASAL
# rm -f params_est.out
# ../../../CASAL/casal -e -o params_est.out > run_estimation.txt 2>&1

# cd ../CASAL_sens1
# rm -f params_est.out
# ../../../CASAL/casal -e -o params_est.out > run_estimation.txt 2>&1


cd Casal2/betadiff_casal_flags_off
rm -f params_est.out
../../../../../BuildSystem/Casal2/casal2 -E mpd.out -o params_est.out > run_estimation.txt 2>&1

cd ../betadiff_casal_flags_on
rm -f params_est.out
../../../../../BuildSystem/Casal2/casal2 -E mpd.out -o params_est.out > run_estimation.txt 2>&1

cd ../betadiff_casal_flags_on_low_tol
rm -f params_est.out
../../../../../BuildSystem/Casal2/casal2 -E mpd.out -o params_est.out > run_estimation.txt 2>&1


cd ../adolc_casal_flags_off
rm -f params_est.out
../../../../../BuildSystem/Casal2/casal2 -E mpd.out -o params_est.out > run_estimation.txt 2>&1

cd ../adolc_casal_flags_on
rm -f params_est.out
../../../../../BuildSystem/Casal2/casal2 -E mpd.out -o params_est.out > run_estimation.txt 2>&1

cd ../adolc_casal_flags_on_low_tol
rm -f params_est.out
../../../../../BuildSystem/Casal2/casal2 -E mpd.out -o params_est.out > run_estimation.txt 2>&1

