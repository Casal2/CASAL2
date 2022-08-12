# this assumes that these scripts have all been run
# ./doBuild.sh thirdparty ; ./doBuild.sh library release ; ./doBuild.sh library adolc ; ./doBuild.sh library betadiff ; ./doBuild.sh library test ; ./doBuild.sh frontend ; ./doBuild.sh archive true
# start in directory TestCases/primary/SBW

# cd CASAL
# rm -f params_est.out
# ../../../CASAL/casal -e -o params_est.out > run_estimation.txt 2>&1

# cd ../CASAL_sens1
# rm -f params_est.out
# ../../../CASAL/casal -e -o params_est.out > run_estimation.txt 2>&1


cd Casal2
rm -f betadiff_params_est.out
../../../../BuildSystem/Casal2/casal2 -E mpd.out -c betadiff_config.csl2 -o betadiff_params_est.out > betadiff.log 2> betadiff.err

rm -f betadiff_low_tolerance_params_est.out
../../../../BuildSystem/Casal2/casal2 -E mpd.out -c betadiff_low_tolerance_config.csl2 -o betadiff_low_tolerance_params_est.out > betadiff_low_tolerance.log 2> betadiff_low_tolerance.err

rm -f adolc_params_est.out
../../../../BuildSystem/Casal2/casal2 -E mpd.out -c adolc_config.csl2 -o adolc_params_est.out > adolc.log 2> adolc.err

rm -f adolc_low_tolerance_params_est.out
../../../../BuildSystem/Casal2/casal2 -E mpd.out -c adolc_low_tolerance_config.csl2 -o adolc_low_tolerance_params_est.out > adolc_low_tolerance.log 2> adolc_low_tolerance.err

