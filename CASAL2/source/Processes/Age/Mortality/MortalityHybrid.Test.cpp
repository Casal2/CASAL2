/**
 * @file MortalityHybrid.Test.cpp
 * @author C Marsh
 * @github https://github.com/Craig44
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 * This test will run Casal2 -r
 * and compare SSBs derived. These SSBs were corroborated by SS equivalent model which is appended at the end of this file.
 * If re-running the SS models. Run them with "ss -stopph 0 -nohess > run.log" this is equivalent to -r in Casal2
 */
#ifdef TESTMODE

// Headers
#include "Model/Model.h"
#include "MortalityHybrid.h"
#include "Processes/Manager.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"
#include "DerivedQuantities/Manager.h"


// Namespaces
namespace niwa {
namespace processes {
namespace age {

using niwa::testfixtures::InternalEmptyModel;

const std::string single_fishery_sexed =
    R"(
@model
start_year 1970
final_year 2001
min_age 1
max_age 40
age_plus true
base_weight_units tonnes
initialisation_phases  iphase1
time_steps spawn_time_step step1 

@categories
format sex 
names male female
age_lengths age_size_male age_size_female

@initialisation_phase iphase1
type derived

@time_step spawn_time_step
processes Nop

@time_step step1
processes Recruitment instant_mort Ageing


@process Recruitment
type recruitment_beverton_holt
categories male female 
proportions 0.5 0.5
r0 18958355 
recruitment_multipliers   	      1 * 32 
steepness 1
ssb biomass_t1
age 1

@process Nop
type null_process

@process Ageing
type ageing
categories *

@process instant_mort
type mortality_hybrid
m 0.15
time_step_proportions  1 
relative_m_by_age One*2
categories *
max_f 2.95
f_iterations 5

table catches
year Fishery
1971	0
1972	200
1973	1000
1974	1000
1975	2000
1976	3000
1977	4000
1978	5000
1979	6000
1980	8000
1981	10000
1982	10000
1983	10000
1984	10000
1985	10000
1986	10000
1987	10000
1988	9000
1989	8000
1990	7000
1991	6000
1992	4000
1993	4000
1994	4000
1995	4000
1996	4000
1997	3000
1998	3000
1999	3000
2000	3000
2001	3000
end_table

table method
method  	category 	selectivity 	annual_duration 	time_step 	penalty
Fishery   	male   		fish_sel 		1	step1 		 none
Fishery  	female   	fish_sel 		1 	step1 		none
end_table

@derived_quantity biomass_t1
type biomass
time_step spawn_time_step
categories  female
time_step_proportion 0
time_step_proportion_method weighted_sum
selectivities MaturationSel

@selectivity MaturationSel
type logistic
a50 5
ato95 3

@selectivity fish_sel
type logistic
a50 4
ato95 3

@selectivity One
type constant
c 1

@age_length age_size_female
type von_bertalanffy
by_length true
time_step_proportions 0 0.5
k 0.167
t0 0
Linf 68
cv_first 0.05
distribution lognormal
length_weight size_weight_female

@age_length age_size_male
type von_bertalanffy
by_length true
time_step_proportions 0 0.5 
k 0.167
t0 0
Linf 68
cv_first 0.05
distribution lognormal
length_weight size_weight_male

@length_weight size_weight_female
type basic
units kgs
a 2.44e-06
b 3.34694

@length_weight size_weight_male
type basic
units kgs
a 2.44e-06
b 3.34694

)";

const std::string two_fisheries_sexed =
    R"(
@model
start_year 1970
final_year 2001
min_age 1
max_age 40
age_plus true
base_weight_units tonnes
initialisation_phases  iphase1
time_steps spawn_time_step step1 

@categories
format sex 
names male female
age_lengths age_size_male age_size_female

@initialisation_phase iphase1
type derived

@time_step spawn_time_step
processes Nop

@time_step step1
processes Recruitment instant_mort Ageing


@process Recruitment
type recruitment_beverton_holt
categories male female 
proportions 0.5 0.5
r0 34544375 
recruitment_multipliers   	      1 * 32 
steepness 1
ssb biomass_t1
age 1

@process Nop
type null_process

@process Ageing
type ageing
categories *

@process instant_mort
type mortality_hybrid
m 0.15
time_step_proportions  1 
relative_m_by_age One*2
categories *
max_f 2.95
f_iterations 5

table catches
year	Fishery1	Fishery2
1971	0	0
1972	200	100
1973	1000	500
1974	1000	500
1975	2000	1000
1976	3000	1500
1977	4000	2000
1978	5000	2500
1979	6000	3000
1980	8000	4000
1981	10000	5000
1982	10000	5000
1983	10000	5000
1984	10000	5000
1985	10000	5000
1986	10000	5000
1987	10000	5000
1988	9000	4500
1989	8000	4000
1990	7000	3500
1991	6000	3000
1992	4000	2000
1993	4000	2000
1994	4000	2000
1995	4000	2000
1996	4000	2000
1997	3000	1500
1998	3000	1500
1999	3000	1500
2000	3000	1500
2001	3000	1500
end_table

table method
method  	category 	selectivity 	annual_duration 	time_step 	penalty
Fishery1   	male   		fish_sel 		1	step1 		 none
Fishery1  	female   	fish_sel 		1 	step1 		 none
Fishery2   	male   		fish_sel_2 		1	step1 		none
Fishery2  	female   	fish_sel_2 		1 	step1 		none

end_table

@derived_quantity biomass_t1
type biomass
time_step spawn_time_step
categories  female
time_step_proportion 0
time_step_proportion_method weighted_sum
selectivities MaturationSel

@selectivity MaturationSel
type logistic
a50 5
ato95 3

@selectivity fish_sel
type logistic
a50 4
ato95 3

@selectivity fish_sel_2
type logistic
a50 5
ato95 3.5

@selectivity One
type constant
c 1

@age_length age_size_female
type von_bertalanffy
by_length true
time_step_proportions 0 0.5
k 0.167
t0 0
Linf 68
cv_first 0.05
distribution lognormal
length_weight size_weight_female

@age_length age_size_male
type von_bertalanffy
by_length true
time_step_proportions 0 0.5 
k 0.167
t0 0
Linf 68
cv_first 0.05
distribution lognormal
length_weight size_weight_male

@length_weight size_weight_female
type basic
units kgs
a 2.44e-06
b 3.34694

@length_weight size_weight_male
type basic
units kgs
a 2.44e-06
b 3.34694

)";

/**
 *
 */
TEST_F(InternalEmptyModel, Processes_Mortality_Hybrid_SingleFishery_Sexed) {
  AddConfigurationLine(single_fishery_sexed, __FILE__, 31);
  LoadConfiguration();
  model_->Start(RunMode::kBasic);

  niwa::DerivedQuantity* dq = model_->managers()->derived_quantity()->GetDerivedQuantity("biomass_t1");
  
  vector<Double> expected_ssbs
      = {58140.1, 58140.1, 58140.1, 58046.3, 57580.8, 57132.4, 56238.1, 54918.6, 53198.2, 51103.9, 48663.7, 45438.5, 41467.9, 37719.5, 34206, 30929.5, 27881.9, 25047, 22403.1, 20381.1, 18963.1, 18132.9, 17876.8, 18638.3, 19527.6, 20494.9, 21496.9, 22500.1, 23941.6, 25368.3, 26755.8, 28084.9};
  
  for (unsigned i = 0; i < expected_ssbs.size(); ++i) {
    unsigned year  = 1970 + i;
    EXPECT_NEAR(expected_ssbs[i], dq->GetValue(year), 1) << " for year " << year << " and value " << expected_ssbs[i];
  }
}

/**
 *
 */
TEST_F(InternalEmptyModel, Processes_Mortality_Hybrid_TwoFishery_Sexed) {
  AddConfigurationLine(two_fisheries_sexed, __FILE__, 31);
  LoadConfiguration();
  model_->Start(RunMode::kBasic);

  niwa::DerivedQuantity* dq = model_->managers()->derived_quantity()->GetDerivedQuantity("biomass_t1");
  
  vector<Double> expected_ssbs
      = {105938, 105938, 105938, 105797, 105098, 104428, 103089, 101117, 98548.3, 95426.1, 91793.4, 86990.6, 81081.8, 75531, 70356.8, 65562.8, 61139.3, 57065.6, 53314.7, 50549.8, 48739.5, 47850.2, 47846.6, 49386.4, 51053.1, 52774.3, 54490.3, 56157.1, 58444.8, 60644.5, 62729.8, 64682.2};
  for (unsigned i = 0; i < expected_ssbs.size(); ++i) {
    unsigned year  = 1970 + i;
    EXPECT_NEAR(expected_ssbs[i], dq->GetValue(year), 1) << " for year " << year << " and value " << expected_ssbs[i];
  }
}

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
#endif /* TESTMODE */

/*
/////////////////////////////////////////////////////////////
////// SingleFsihery model
//////////////////////////////////////////////////////////////
// only the Control and Data ss files are needed to re-run the results
////////// Control.ss
#V3.30.17.00;_2021_06_11;_safe;_Stock_Synthesis_by_Richard_Methot_(NOAA)_using_ADMB_12.3
#Stock Synthesis (SS) is a work of the U.S. Government and is not subject to copyright protection in the United States.
#Foreign copyrights may apply. See copyright.txt for more information.
#_user_support_available_at:NMFS.Stock.Synthesis@noaa.gov
#_user_info_available_at:https://vlab.noaa.gov/group/stock-synthesis
#C growth parameters are estimated
#C spawner-recruitment bias adjustment Not tuned For optimality
#_data_and_control_files: data.ss // control.ss
0  # 0 means do not read wtatage.ss; 1 means read and use wtatage.ss and also read and use growth parameters
1  #_N_Growth_Patterns (Growth Patterns, Morphs, Bio Patterns, GP are terms used interchangeably in SS)
1 #_N_platoons_Within_GrowthPattern 
#_Cond 1 #_Platoon_within/between_stdev_ratio (no read if N_platoons=1)
#_Cond  1 #vector_platoon_dist_(-1_in_first_val_gives_normal_approx)
#
2 # recr_dist_method for parameters:  2=main effects for GP, Area, Settle timing; 3=each Settle entity; 4=none (only when N_GP*Nsettle*pop==1)
1 # not yet implemented; Future usage: Spawner-Recruitment: 1=global; 2=by area
1 #  number of recruitment settlement assignments 
0 # unused option
#GPattern month  area  age (for each settlement assignment)
 1 1 1 0
#
#_Cond 0 # N_movement_definitions goes here if Nareas > 1
#_Cond 1.0 # first age that moves (real age at begin of season, not integer) also cond on do_migration>0
#_Cond 1 1 1 2 4 10 # example move definition for seas=1, morph=1, source=1 dest=2, age1=4, age2=10
#
1 #_Nblock_Patterns
 1 #_blocks_per_pattern 
# begin and end years of blocks
 1970 1970
#
# controls for all timevary parameters 
1 #_time-vary parm bound check (1=warn relative to base parm bounds; 3=no bound check); Also see env (3) and dev (5) options to constrain with base bounds
#
# AUTOGEN
 0 0 0 0 0 # autogen: 1st element for biology, 2nd for SR, 3rd for Q, 4th reserved, 5th for selex
# where: 0 = autogen time-varying parms of this category; 1 = read each time-varying parm line; 2 = read then autogen if parm min==-12345
#
#_Available timevary codes
#_Block types: 0: P_block=P_base*exp(TVP); 1: P_block=P_base+TVP; 2: P_block=TVP; 3: P_block=P_block(-1) + TVP
#_Block_trends: -1: trend bounded by base parm min-max and parms in transformed units (beware); -2: endtrend and infl_year direct values; -3: end and infl as fraction of base range
#_EnvLinks:  1: P(y)=P_base*exp(TVP*env(y));  2: P(y)=P_base+TVP*env(y);  3: P(y)=f(TVP,env_Zscore) w/ logit to stay in min-max;  4: P(y)=2.0/(1.0+exp(-TVP1*env(y) - TVP2))
#_DevLinks:  1: P(y)*=exp(dev(y)*dev_se;  2: P(y)+=dev(y)*dev_se;  3: random walk;  4: zero-reverting random walk with rho;  5: like 4 with logit transform to stay in base min-max
#_DevLinks(more):  21-25 keep last dev for rest of years
#
#_Prior_codes:  0=none; 6=normal; 1=symmetric beta; 2=CASAL's beta; 3=lognormal; 4=lognormal with biascorr; 5=gamma
#
# setup for M, growth, wt-len, maturity, fecundity, (hermaphro), recr_distr, cohort_grow, (movement), (age error), (catch_mult), sex ratio 
#_NATMORT
0 #_natM_type:_0=1Parm; 1=N_breakpoints;_2=Lorenzen;_3=agespecific;_4=agespec_withseasinterpolate;_5=BETA:_Maunder_link_to_maturity
  #_no additional input for selected M option; read 1P per morph
#
1 # GrowthModel: 1=vonBert with L1&L2; 2=Richards with L1&L2; 3=age_specific_K_incr; 4=age_specific_K_decr; 5=age_specific_K_each; 6=NA; 7=NA; 8=growth cessation
0 #_Age(post-settlement)_for_L1;linear growth below this
999 #_Growth_Age_for_L2 (999 to use as Linf)
-999 #_exponential decay for growth above maxage (value should approx initial Z; -999 replicates 3.24; -998 to not allow growth above maxage)
0  #_placeholder for future growth feature
#
0 #_SD_add_to_LAA (set to 0.1 for SS2 V1.x compatibility)
0 #_CV_Growth_Pattern:  0 CV=f(LAA); 1 CV=F(A); 2 SD=F(LAA); 3 SD=F(A); 4 logSD=F(A)
#
2 #_maturity_option:  1=length logistic; 2=age logistic; 3=read age-maturity matrix by growth_pattern; 4=read age-fecundity; 5=disabled; 6=read length-maturity
1 #_First_Mature_Age
1 #_fecundity option:(1)eggs=Wt*(a+b*Wt);(2)eggs=a*L^b;(3)eggs=a*Wt^b; (4)eggs=a+b*L; (5)eggs=a+b*W
0 #_hermaphroditism option:  0=none; 1=female-to-male age-specific fxn; -1=male-to-female age-specific fxn
1 #_parameter_offset_approach for M, G, CV_G:  1- direct, no offset**; 2- male=fem_parm*exp(male_parm); 3: male=female*exp(parm) then old=young*exp(parm)
#_** in option 1, any male parameter with value = 0.0 and phase <0 is set equal to female parameter
#
#_growth_parms
#_ LO HI INIT PRIOR PR_SD PR_type PHASE env_var&link dev_link dev_minyr dev_maxyr dev_PH Block Block_Fxn
# Sex: 1  BioPattern: 1  NatMort
 0.05 0.15 0.15 0.1 0.8 0 -3 0 0 0 0 0 0 0 # NatM_uniform_Fem_GP_1
# Sex: 1  BioPattern: 1  Growth
 -10 45 0 36 10 6 2 0 0 0 0 0 0 0 # L_at_Amin_Fem_GP_1
 40 90 68 70 10 6 4 0 0 0 0 0 0 0 # L_at_Amax_Fem_GP_1
 0.05 0.25 0.167 0.15 0.8 6 4 0 0 0 0 0 0 0 # VonBert_K_Fem_GP_1
 0.05 0.25 0.05 0.1 0.8 0 -3 0 0 0 0 0 0 0 # CV_young_Fem_GP_1
 0.05 0.25 0.05 0.1 0.8 0 -3 0 0 0 0 0 0 0 # CV_old_Fem_GP_1
# Sex: 1  BioPattern: 1  WtLen
 -3 3 2.44e-06 2.44e-06 0.8 0 -3 0 0 0 0 0 0 0 # Wtlen_1_Fem_GP_1
 -3 4 3.34694 3.34694 0.8 0 -3 0 0 0 0 0 0 0 # Wtlen_2_Fem_GP_1
# Sex: 1  BioPattern: 1  Maturity&Fecundity
0 60 5 55 0.8 0 -3 0 0 0 0 0 0 0 # Mat50%_Fem_GP_1
 -3 3 -0.99 -0.25 0.8 0 -3 0 0 0 0 0 0 0 # Mat_slope_Fem_GP_1
 -3 3 1 1 0.8 0 -3 0 0 0 0 0 0 0 # Eggs/kg_inter_Fem_GP_1
 -3 3 0 0 0.8 0 -3 0 0 0 0 0 0 0 # Eggs/kg_slope_wt_Fem_GP_1
# Sex: 2  BioPattern: 1  NatMort
 0.05 0.15 0.15 0.1 0.8 0 -3 0 0 0 0 0 0 0 # NatM_uniform_Mal_GP_1
# Sex: 2  BioPattern: 1  Growth
 0 45 0 36 10 0 -3 0 0 0 0 0 0 0 # L_at_Amin_Mal_GP_1
 40 90 68 70 10 6 4 0 0 0 0 0 0 0 # L_at_Amax_Mal_GP_1
 0.05 0.25 0.167 0.15 0.8 6 4 0 0 0 0 0 0 0 # VonBert_K_Mal_GP_1
 0.05 0.25 0.05 0.1 0.8 0 -3 0 0 0 0 0 0 0 # CV_young_Mal_GP_1
 0.05 0.25 0.05 0.1 0.8 0 -3 0 0 0 0 0 0 0 # CV_old_Mal_GP_1
# Sex: 2  BioPattern: 1  WtLen
 -3 3 2.44e-06 2.44e-06 0.8 0 -3 0 0 0 0 0 0 0 # Wtlen_1_Mal_GP_1
 -3 4 3.34694 3.34694 0.8 0 -3 0 0 0 0 0 0 0 # Wtlen_2_Mal_GP_1
# Hermaphroditism
#  Recruitment Distribution  
 0 0 0 0 0 0 -4 0 0 0 0 0 0 0 # RecrDist_GP_1
 0 0 0 0 0 0 -4 0 0 0 0 0 0 0 # RecrDist_Area_1
 0 0 0 0 0 0 -4 0 0 0 0 0 0 0 # RecrDist_month_1
#  Cohort growth dev base
 0.1 10 1 1 1 0 -1 0 0 0 0 0 0 0 # CohortGrowDev
#  Movement
#  Age Error from parameters
#  catch multiplier
#  fraction female, by GP
 1e-06 0.999999 0.5 0.5 0.5 0 -99 0 0 0 0 0 0 0 # FracFemale_GP_1
#
#_no timevary MG parameters
#
#_seasonal_effects_on_biology_parms
 0 0 0 0 0 0 0 0 0 0 #_femwtlen1,femwtlen2,mat1,mat2,fec1,fec2,Malewtlen1,malewtlen2,L1,K
#_ LO HI INIT PRIOR PR_SD PR_type PHASE
#_Cond -2 2 0 0 -1 99 -2 #_placeholder when no seasonal MG parameters
#
3 #_Spawner-Recruitment; Options: 1=NA; 2=Ricker; 3=std_B-H; 4=SCAA; 5=Hockey; 6=B-H_flattop; 7=survival_3Parm; 8=Shepherd_3Parm; 9=RickerPower_3parm
0  # 0/1 to use steepness in initial equ recruitment calculation
0  #  future feature:  0/1 to make realized sigmaR a function of SR curvature
#_          LO            HI          INIT         PRIOR         PR_SD       PR_type      PHASE    env-var    use_dev   dev_mnyr   dev_mxyr     dev_PH      Block    Blk_Fxn #  parm_name
             3            31       10          10.3            10             0          1          0          0          0          0          0          0          0 # SR_LN(R0)
           0.2             1      		1           0.7          0.05             1          4          0          0          0          0          0          0          0 # SR_BH_steep
             0             2           0.6           0.8           0.8             0         -4          0          0          0          0          0          0          0 # SR_sigmaR
            -5             5             0             0             1             0         -4          0          0          0          0          0          0          0 # SR_regime
             0             0             0             0             0             0        -99          0          0          0          0          0          0          0 # SR_autocorr
#_no timevary SR parameters
1 #do_recdev:  0=none; 1=devvector (R=F(SSB)+dev); 2=deviations (R=F(SSB)+dev); 3=deviations (R=R0*dev; dev2=R-f(SSB)); 4=like 3 with sum(dev2) adding penalty
1971 # first year of main recr_devs; early devs can preceed this era
2001 # last year of main recr_devs; forecast devs start in following year
2 #_recdev phase 
1 # (0/1) to read 13 advanced options
 0 #_recdev_early_start (0=none; neg value makes relative to recdev_start)
 -4 #_recdev_early_phase
 0 #_forecast_recruitment phase (incl. late recr) (0 value resets to maxphase+1)
 1 #_lambda for Fcast_recr_like occurring before endyr+1
 1900 #_last_yr_nobias_adj_in_MPD; begin of ramp
 1900 #_first_yr_fullbias_adj_in_MPD; begin of plateau
 2001 #_last_yr_fullbias_adj_in_MPD
 2002 #_end_yr_for_ramp_in_MPD (can be in forecast to shape ramp, but SS sets bias_adj to 0.0 for fcast yrs)
 0 #_max_bias_adj_in_MPD (typical ~0.8; -3 sets all years to 0.0; -2 sets all non-forecast yrs w/ estimated recdevs to 1.0; -1 sets biasadj=1.0 for all yrs w/ recdevs)
 0 #_period of cycles in recruitment (N parms read below)
 -5 #min rec_dev
 5 #max rec_dev
 0 #_read_recdevs
#_end of advanced SR options
#
#_placeholder for full parameter lines for recruitment cycles
# read specified recr devs
#_Yr Input_value
#
# all recruitment deviations
#  1971R 1972R 1973R 1974R 1975R 1976R 1977R 1978R 1979R 1980R 1981R 1982R 1983R 1984R 1985R 1986R 1987R 1988R 1989R 1990R 1991R 1992R 1993R 1994R 1995R 1996R 1997R 1998R 1999R 2000R 2001R 2002F 2003F 2004F 2005F 2006F 2007F 2008F 2009F 2010F 2011F
#  0.1268 -0.0629442 0.0998014 -0.174095 0.0306484 0.714818 -0.0228752 0.00379775 0.261267 0.173626 0.0900049 -0.226622 -0.439888 -0.312088 0.393112 0.551725 0.21891 0.154932 -0.384786 0.596744 -0.682432 -0.273424 -0.829665 0.365024 -0.605267 0.455103 1.11072 -0.546499 -0.656469 0.171606 -0.301581 0 0 0 0 0 0 0 0 0 0
#
#Fishing Mortality info 
0.3 # F ballpark value in units of annual_F
-2001 # F ballpark year (neg value to disable)
3 # F_Method:  1=Pope; 2=instan. F; 3=hybrid (hybrid is recommended)
2.95 # max F or harvest rate, depends on F_Method
# no additional F input needed for Fmethod 1
# if Fmethod=2; read overall start F value; overall phase; N detailed inputs to read
# if Fmethod=3; read N iterations for tuning for Fmethod 3
4  # N iterations for tuning F in hybrid method (recommend 3 to 7)
#
#_initial_F_parms; for each fleet x season that has init_catch; nest season in fleet; count = 0
#_for unconstrained init_F, use an arbitrary initial catch and set lambda=0 for its logL
#_ LO HI INIT PRIOR PR_SD  PR_type  PHASE
#
# F rates by fleet x season
# Yr:  1971 1972 1973 1974 1975 1976 1977 1978 1979 1980 1981 1982 1983 1984 1985 1986 1987 1988 1989 1990 1991 1992 1993 1994 1995 1996 1997 1998 1999 2000 2001 2002 2003 2004 2005 2006 2007 2008 2009 2010 2011
# seas:  1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
# FISHERY 0 0.00211081 0.010609 0.0107037 0.0217063 0.0333334 0.0459509 0.0599453 0.0757167 0.107737 0.146876 0.162531 0.180868 0.202893 0.230365 0.266192 0.314644 0.338215 0.354481 0.356016 0.338877 0.238035 0.242891 0.250688 0.26355 0.283377 0.227156 0.238194 0.247552 0.252337 0.253174 0.0129829 0.0279253 0.038022 0.0447387 0.0493313 0.0527091 0.0554663 0.0579281 0.0602317 0.0624094
#
#_Q_setup for fleets with cpue or survey data
#_1:  fleet number
#_2:  link type: (1=simple q, 1 parm; 2=mirror simple q, 1 mirrored parm; 3=q and power, 2 parm; 4=mirror with offset, 2 parm)
#_3:  extra input for link, i.e. mirror fleet# or dev index number
#_4:  0/1 to select extra sd parameter
#_5:  0/1 for biasadj or not
#_6:  0/1 to float
#_   fleet      link link_info  extra_se   biasadj     float  #  fleetname
         2         1         0         1         0         0  #  SURVEY1
         3         1         0         0         0         0  #  SURVEY2
-9999 0 0 0 0 0
#
#_Q_parms(if_any);Qunits_are_ln(q)
#_          LO            HI          INIT         PRIOR         PR_SD       PR_type      PHASE    env-var    use_dev   dev_mnyr   dev_mxyr     dev_PH      Block    Blk_Fxn  #  parm_name
            -7             5      0.516018             0             1             0          1          0          0          0          0          0          0          0  #  LnQ_base_SURVEY1(2)
             0           0.5             0          0.05             1             0         -4          0          0          0          0          0          0          0  #  Q_extraSD_SURVEY1(2)
            -7             5       -6.6281             0             1             0          1          0          0          0          0          0          0          0  #  LnQ_base_SURVEY2(3)
#_no timevary Q parameters
#
#_size_selex_patterns
#Pattern:_0;  parm=0; selex=1.0 for all sizes
#Pattern:_1;  parm=2; logistic; with 95% width specification
#Pattern:_2;  parm=6; modification of pattern 24 with improved sex-specific offset
#Pattern:_5;  parm=2; mirror another size selex; PARMS pick the min-max bin to mirror
#Pattern:_11; parm=2; selex=1.0  for specified min-max population length bin range
#Pattern:_15; parm=0; mirror another age or length selex
#Pattern:_6;  parm=2+special; non-parm len selex
#Pattern:_43; parm=2+special+2;  like 6, with 2 additional param for scaling (average over bin range)
#Pattern:_8;  parm=8; double_logistic with smooth transitions and constant above Linf option
#Pattern:_9;  parm=6; simple 4-parm double logistic with starting length; parm 5 is first length; parm 6=1 does desc as offset
#Pattern:_21; parm=2+special; non-parm len selex, read as pairs of size, then selex
#Pattern:_22; parm=4; double_normal as in CASAL
#Pattern:_23; parm=6; double_normal where final value is directly equal to sp(6) so can be >1.0
#Pattern:_24; parm=6; double_normal with sel(minL) and sel(maxL), using joiners
#Pattern:_25; parm=3; exponential-logistic in length
#Pattern:_27; parm=special+3; cubic spline in length; parm1==1 resets knots; parm1==2 resets all 
#Pattern:_42; parm=special+3+2; cubic spline; like 27, with 2 additional param for scaling (average over bin range)
#_discard_options:_0=none;_1=define_retention;_2=retention&mortality;_3=all_discarded_dead;_4=define_dome-shaped_retention
#_Pattern Discard Male Special
 0 0 0 0 # 1 FISHERY
 1 0 0 0 # 2 SURVEY1
 0 0 0 0 # 3 SURVEY2
#
#_age_selex_patterns
#Pattern:_0; parm=0; selex=1.0 for ages 0 to maxage
#Pattern:_10; parm=0; selex=1.0 for ages 1 to maxage
#Pattern:_11; parm=2; selex=1.0  for specified min-max age
#Pattern:_12; parm=2; age logistic
#Pattern:_13; parm=8; age double logistic
#Pattern:_14; parm=nages+1; age empirical
#Pattern:_15; parm=0; mirror another age or length selex
#Pattern:_16; parm=2; Coleraine - Gaussian
#Pattern:_17; parm=nages+1; empirical as random walk  N parameters to read can be overridden by setting special to non-zero
#Pattern:_41; parm=2+nages+1; // like 17, with 2 additional param for scaling (average over bin range)
#Pattern:_18; parm=8; double logistic - smooth transition
#Pattern:_19; parm=6; simple 4-parm double logistic with starting age
#Pattern:_20; parm=6; double_normal,using joiners
#Pattern:_26; parm=3; exponential-logistic in age
#Pattern:_27; parm=3+special; cubic spline in age; parm1==1 resets knots; parm1==2 resets all 
#Pattern:_42; parm=2+special+3; // cubic spline; with 2 additional param for scaling (average over bin range)
#Age patterns entered with value >100 create Min_selage from first digit and pattern from remainder
#_Pattern Discard Male Special
 12 0 0 0 # 1 FISHERY
 11 0 0 0 # 2 SURVEY1
 11 0 0 0 # 3 SURVEY2
#
#_          LO            HI          INIT         PRIOR         PR_SD       PR_type      PHASE    env-var    use_dev   dev_mnyr   dev_mxyr     dev_PH      Block    Blk_Fxn  #  parm_name
# 1   FISHERY LenSelex
#            19            80       53.6411            50          0.01             1          2          0          0          0          0          0          0          0  #  Size_inflection_FISHERY(1)
 #         0.01            60       18.9232            15          0.01             1          3          0          0          0          0          0          0          0  #  Size_95%width_FISHERY(1)
# 2   SURVEY1 LenSelex
            19            70        36.653            30          0.01             1          2          0          0          0          0          0          0          0  #  Size_inflection_SURVEY1(2)
          0.01            60       6.59179            10          0.01             1          3          0          0          0          0          0          0          0  #  Size_95%width_SURVEY1(2)
# 3   SURVEY2 LenSelex
# 1   FISHERY AgeSelex
             0            40             4             5            99             0        3         0          0          0          0          0          0          0  #  minage@sel=1_FISHERY(1)
             0            40            3             6            99             0        3          0          0          0          0          0          0          0  #  maxage@sel=1_FISHERY(1)
# 2   SURVEY1 AgeSelex
             0            40             0             5            99             0        -99          0          0          0          0          0          0          0  #  minage@sel=1_SURVEY1(2)
             0            40            40             6            99             0        -99          0          0          0          0          0          0          0  #  maxage@sel=1_SURVEY1(2)
# 3   SURVEY2 AgeSelex
             0            40             0             5            99             0        -99          0          0          0          0          0          0          0  #  minage@sel=1_SURVEY2(3)
             0            40             0             6            99             0        -99          0          0          0          0          0          0          0  #  maxage@sel=1_SURVEY2(3)
#_No_Dirichlet parameters
#_no timevary selex parameters
#
0   #  use 2D_AR1 selectivity(0/1)
#_no 2D_AR1 selex offset used
#
# Tag loss and Tag reporting parameters go next
0  # TG_custom:  0=no read and autogen if tag data exist; 1=read
#_Cond -6 6 1 1 2 0.01 -4 0 0 0 0 0 0 0  #_placeholder if no parameters
#
# no timevary parameters
#
#
# Input variance adjustments factors: 
 #_1=add_to_survey_CV
 #_2=add_to_discard_stddev
 #_3=add_to_bodywt_CV
 #_4=mult_by_lencomp_N
 #_5=mult_by_agecomp_N
 #_6=mult_by_size-at-age_N
 #_7=mult_by_generalized_sizecomp
#_Factor  Fleet  Value
 -9999   1    0  # terminator
#
4 #_maxlambdaphase
1 #_sd_offset; must be 1 if any growthCV, sigmaR, or survey extraSD is an estimated parameter
# read 3 changes to default Lambdas (default value is 1.0)
# Like_comp codes:  1=surv; 2=disc; 3=mnwt; 4=length; 5=age; 6=SizeFreq; 7=sizeage; 8=catch; 9=init_equ_catch; 
# 10=recrdev; 11=parm_prior; 12=parm_dev; 13=CrashPen; 14=Morphcomp; 15=Tag-comp; 16=Tag-negbin; 17=F_ballpark; 18=initEQregime
#like_comp fleet  phase  value  sizefreq_method
 1 2 2 1 1
 4 2 2 1 1
 4 2 3 1 1
-9999  1  1  1  1  #  terminator
#
# lambdas (for info only; columns are phases)
#  0 0 0 0 #_CPUE/survey:_1
#  1 1 1 1 #_CPUE/survey:_2
#  1 1 1 1 #_CPUE/survey:_3
#  1 1 1 1 #_lencomp:_1
#  1 1 1 1 #_lencomp:_2
#  0 0 0 0 #_lencomp:_3
#  1 1 1 1 #_agecomp:_1
#  1 1 1 1 #_agecomp:_2
#  0 0 0 0 #_agecomp:_3
#  1 1 1 1 #_size-age:_1
#  1 1 1 1 #_size-age:_2
#  0 0 0 0 #_size-age:_3
#  1 1 1 1 #_init_equ_catch1
#  1 1 1 1 #_init_equ_catch2
#  1 1 1 1 #_init_equ_catch3
#  1 1 1 1 #_recruitments
#  1 1 1 1 #_parameter-priors
#  1 1 1 1 #_parameter-dev-vectors
#  1 1 1 1 #_crashPenLambda
#  0 0 0 0 # F_ballpark_lambda
1 # (0/1/2) read specs for more stddev reporting: 0 = skip, 1 = read specs for reporting stdev for selectivity, size, and numbers, 2 = add options for M,Dyn. Bzero, SmryBio
 1 1 -1 5 # Selectivity: (1) 0 to skip or fleet, (2) 1=len/2=age/3=combined, (3) year, (4) N selex bins; NOTE: combined reports in age bins
 1 5 # Growth: (1) 0 to skip or growth pattern, (2) growth ages; NOTE: does each sex
 1 -1 5 # Numbers-at-age: (1) 0 or area(-1 for all), (2) year, (3) N ages;  NOTE: sums across morphs
 5 15 25 35 43 # vector with selex std bins (-1 in first bin to self-generate)
 1 2 14 26 40 # vector with growth std ages picks (-1 in first bin to self-generate)
 1 2 14 26 40 # vector with NatAge std ages (-1 in first bin to self-generate)
999
///////////////// data.ss
#V3.30.17.00;_2021_06_11;_safe;_Stock_Synthesis_by_Richard_Methot_(NOAA)_using_ADMB_12.3
#Stock Synthesis (SS) is a work of the U.S. Government and is not subject to copyright protection in the United States.
#Foreign copyrights may apply. See copyright.txt for more information.
#_user_support_available_at:NMFS.Stock.Synthesis@noaa.gov
#_user_info_available_at:https://vlab.noaa.gov/group/stock-synthesis
#_Start_time: Fri Jun 11 08:38:55 2021
#_Number_of_datafiles: 1
#C data file for simple example
#_observed data: 
#V3.30.17.00;_2021_06_11;_safe;_Stock_Synthesis_by_Richard_Methot_(NOAA)_using_ADMB_12.3
#Stock Synthesis (SS) is a work of the U.S. Government and is not subject to copyright protection in the United States.
#Foreign copyrights may apply. See copyright.txt for more information.
1971 #_StartYr
2001 #_EndYr
1 #_Nseas
12 #_months/season
2 #_Nsubseasons (even number, minimum is 2)
0 #_spawn_month
2 #_Ngenders: 1, 2, -1  (use -1 for 1 sex setup with SSB multiplied by female_frac parameter)
40 #_Nages=accumulator age, first age is always age 0
1 #_Nareas
3 #_Nfleets (including surveys)
#_fleet_type: 1=catch fleet; 2=bycatch only fleet; 3=survey; 4=ignore 
#_sample_timing: -1 for fishing fleet to use season-long catch-at-age for observations, or 1 to use observation month;  (always 1 for surveys)
#_fleet_area:  area the fleet/survey operates in 
#_units of catch:  1=bio; 2=num (ignored for surveys; their units read later)
#_catch_mult: 0=no; 1=yes
#_rows are fleets
#_fleet_type fishery_timing area catch_units need_catch_mult fleetname
 1 -1 1 1 0 FISHERY  # 1
 3 1 1 2 0 SURVEY1  # 2
 3 1 1 2 0 SURVEY2  # 3
#Bycatch_fleet_input_goes_next
#a:  fleet index
#b:  1=include dead bycatch in total dead catch for F0.1 and MSY optimizations and forecast ABC; 2=omit from total catch for these purposes (but still include the mortality)
#c:  1=Fmult scales with other fleets; 2=bycatch F constant at input value; 3=bycatch F from range of years
#d:  F or first year of range
#e:  last year of range
#f:  not used
# a   b   c   d   e   f 
#_Catch data: yr, seas, fleet, catch, catch_se
#_catch_se:  standard error of log(catch)
#_NOTE:  catch data is ignored for survey fleets
-999	1	1	0	0.01
1971	1	1	0	0.01
1972	1	1	200	0.01
1973	1	1	1000	0.01
1974	1	1	1000	0.01
1975	1	1	2000	0.01
1976	1	1	3000	0.01
1977	1	1	4000	0.01
1978	1	1	5000	0.01
1979	1	1	6000	0.01
1980	1	1	8000	0.01
1981	1	1	10000	0.01
1982	1	1	10000	0.01
1983	1	1	10000	0.01
1984	1	1	10000	0.01
1985	1	1	10000	0.01
1986	1	1	10000	0.01
1987	1	1	10000	0.01
1988	1	1	9000	0.01
1989	1	1	8000	0.01
1990	1	1	7000	0.01
1991	1	1	6000	0.01
1992	1	1	4000	0.01
1993	1	1	4000	0.01
1994	1	1	4000	0.01
1995	1	1	4000	0.01
1996	1	1	4000	0.01
1997	1	1	3000	0.01
1998	1	1	3000	0.01
1999	1	1	3000	0.01
2000	1	1	3000	0.01
2001	1	1	3000	0.01
-9999	0	0	0	0

#
 #_CPUE_and_surveyabundance_observations
#_Units:  0=numbers; 1=biomass; 2=F; 30=spawnbio; 31=recdev; 32=spawnbio*recdev; 33=recruitment; 34=depletion(&see Qsetup); 35=parm_dev(&see Qsetup)
#_Errtype:  -1=normal; 0=lognormal; >0=T
#_SD_Report: 0=no sdreport; 1=enable sdreport
#_Fleet Units Errtype SD_Report
1 1 0 0 # FISHERY
2 1 0 1 # SURVEY1
3 0 0 0 # SURVEY2
#_yr month fleet obs stderr
1977 7 2 339689 0.3 #_ SURVEY1
1980 7 2 193353 0.3 #_ SURVEY1
1983 7 2 151984 0.3 #_ SURVEY1
1986 7 2 55221.8 0.3 #_ SURVEY1
1989 7 2 59232.3 0.3 #_ SURVEY1
1992 7 2 31137.5 0.3 #_ SURVEY1
1995 7 2 35845.4 0.3 #_ SURVEY1
1998 7 2 27492.6 0.3 #_ SURVEY1
2001 7 2 37338.3 0.3 #_ SURVEY1
1990 7 3 5.19333 0.7 #_ SURVEY2
1991 7 3 1.1784 0.7 #_ SURVEY2
1992 7 3 5.94383 0.7 #_ SURVEY2
1993 7 3 0.770106 0.7 #_ SURVEY2
1994 7 3 16.318 0.7 #_ SURVEY2
1995 7 3 1.36339 0.7 #_ SURVEY2
1996 7 3 4.76482 0.7 #_ SURVEY2
1997 7 3 51.0707 0.7 #_ SURVEY2
1998 7 3 1.36095 0.7 #_ SURVEY2
1999 7 3 0.862531 0.7 #_ SURVEY2
2000 7 3 5.97125 0.7 #_ SURVEY2
2001 7 3 1.69379 0.7 #_ SURVEY2
-9999 1 1 1 1 # terminator for survey observations 
#
0 #_N_fleets_with_discard
#_discard_units (1=same_as_catchunits(bio/num); 2=fraction; 3=numbers)
#_discard_errtype:  >0 for DF of T-dist(read CV below); 0 for normal with CV; -1 for normal with se; -2 for lognormal; -3 for trunc normal with CV
# note: only enter units and errtype for fleets with discard 
# note: discard data is the total for an entire season, so input of month here must be to a month in that season
#_Fleet units errtype
# -9999 0 0 0.0 0.0 # terminator for discard data 
#
0 #_use meanbodysize_data (0/1)
#_COND_0 #_DF_for_meanbodysize_T-distribution_like
# note:  type=1 for mean length; type=2 for mean body weight 
#_yr month fleet part type obs stderr
#  -9999 0 0 0 0 0 0 # terminator for mean body size data 
#
# set up population length bin structure (note - irrelevant if not using size data and using empirical wtatage
2 # length bin method: 1=use databins; 2=generate from binwidth,min,max below; 3=read vector
2 # binwidth for population size comp 
10 # minimum size in the population (lower edge of first bin and size at age 0.00) 
94 # maximum size in the population (lower edge of last bin) 
1 # use length composition data (0/1)
#_mintailcomp: upper and lower distribution for females and males separately are accumulated until exceeding this level.
#_addtocomp:  after accumulation of tails; this value added to all bins
#_combM+F: males and females treated as combined gender below this bin number 
#_compressbins: accumulate upper tail by this number of bins; acts simultaneous with mintailcomp; set=0 for no forced accumulation
#_Comp_Error:  0=multinomial, 1=dirichlet
#_ParmSelect:  parm number for dirichlet
#_minsamplesize: minimum sample size; set to 1 to match 3.24, minimum value is 0.001
#
#_mintailcomp addtocomp combM+F CompressBins CompError ParmSelect minsamplesize
0 1e-07 0 0 0 0 1 #_fleet:1_FISHERY
0 1e-07 0 0 0 0 1 #_fleet:2_SURVEY1
0 1e-07 0 0 0 0 1 #_fleet:3_SURVEY2
# sex codes:  0=combined; 1=use female only; 2=use male only; 3=use both as joint sexxlength distribution
# partition codes:  (0=combined; 1=discard; 2=retained
25 #_N_LengthBins; then enter lower edge of each length bin
 26 28 30 32 34 36 38 40 42 44 46 48 50 52 54 56 58 60 62 64 68 72 76 80 90
#_yr month fleet sex part Nsamp datavector(female-male)
 1971 7 1 3 0 125 0 0 0 0 0 0 0 0 0 4 1 1 2 4 1 5 6 2 3 11 8 4 5 0 0 0 0 0 0 0 0 0 0 1 0 1 3 0 3 4 2 4 5 9 17 8 3 8 0 0
 1972 7 1 3 0 125 0 0 0 0 0 0 0 0 0 3 0 1 2 1 1 6 2 7 4 10 10 4 5 3 0 0 0 0 0 0 0 0 0 1 3 2 4 1 3 1 4 4 7 3 8 11 4 10 0 0
 1973 7 1 3 0 125 0 0 0 0 0 0 0 0 0 0 0 0 7 3 4 5 6 3 10 12 6 10 9 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 1 3 0 7 2 6 7 8 5 5 3 0
 1974 7 1 3 0 125 0 0 0 0 0 0 0 0 0 2 2 0 1 1 1 4 5 3 8 8 10 4 7 0 0 0 0 0 0 0 0 0 1 2 0 4 0 0 1 5 6 6 4 6 15 11 5 0 3 0
 1975 7 1 3 0 125 0 0 0 0 0 0 0 2 1 2 1 1 3 0 2 5 6 2 3 5 9 10 10 0 0 0 0 0 0 0 0 0 0 0 4 2 2 1 2 3 5 1 4 5 13 11 6 4 0 0
 1976 7 1 3 0 125 0 0 0 0 0 0 0 2 1 0 2 2 0 3 2 3 3 3 7 18 14 4 2 2 0 0 0 0 0 0 0 1 0 0 0 0 0 1 2 4 6 6 5 7 12 6 4 3 0 0
 1977 7 1 3 0 125 0 0 0 0 0 0 0 1 0 2 0 2 2 4 0 2 6 7 5 11 7 8 5 4 0 0 0 0 0 0 2 1 3 0 1 3 3 2 0 1 4 5 3 7 7 9 5 3 0 0
 1978 7 1 3 0 125 0 0 0 0 0 0 5 1 1 1 0 1 3 1 8 4 4 6 5 9 8 3 6 5 0 0 0 0 0 0 0 0 2 1 1 2 1 1 2 2 4 1 4 1 13 9 6 4 0 0
 1979 7 1 3 0 125 0 0 0 0 0 0 0 0 0 0 3 5 2 1 5 0 5 5 2 7 4 7 5 5 0 0 0 0 0 0 0 0 0 2 2 1 3 2 7 2 4 4 5 8 10 8 6 4 1 0
 1980 7 1 3 0 125 0 0 0 0 0 0 0 4 0 0 1 0 2 4 3 2 3 2 3 16 11 12 4 2 0 0 0 0 0 0 0 0 0 1 4 1 1 2 3 5 2 6 3 1 10 11 4 2 0 0
 1981 7 1 3 0 125 0 0 0 0 0 0 1 0 0 0 3 1 2 2 4 5 2 7 3 13 9 8 4 0 0 0 0 0 0 0 2 1 1 1 2 2 3 3 1 6 1 2 1 7 5 10 6 7 0 0
 1982 7 1 3 0 125 0 0 0 0 0 0 0 0 5 2 1 3 2 3 8 2 5 4 4 6 10 11 0 0 0 0 0 0 0 0 0 0 1 0 3 0 2 1 5 6 1 8 5 5 10 5 2 5 0 0
 1983 7 1 3 0 125 0 0 0 0 0 0 0 0 0 0 7 1 1 5 4 2 2 6 2 8 13 8 6 0 0 0 0 0 0 0 0 0 0 0 4 1 0 3 3 0 4 9 5 4 7 8 6 6 0 0
 1984 7 1 3 0 125 0 0 0 0 0 0 1 0 0 4 3 0 3 1 2 5 2 4 7 11 9 6 8 0 0 0 0 0 0 0 0 0 3 3 1 1 3 3 3 2 2 4 4 8 11 4 5 2 0 0
 1985 7 1 3 0 125 0 0 0 0 0 0 0 0 1 1 2 2 5 0 3 3 5 11 4 8 9 3 2 4 0 0 0 0 0 0 0 0 1 0 1 2 0 3 8 3 4 3 8 4 13 7 4 1 0 0
 1986 7 1 3 0 125 0 0 0 3 1 0 1 2 0 4 2 0 0 4 2 8 3 5 11 5 6 6 1 0 0 0 0 0 0 0 2 2 0 1 2 1 3 4 2 3 4 6 5 5 6 4 6 5 0 0
 1987 7 1 3 0 125 0 0 0 0 1 1 1 1 1 0 2 1 6 4 2 7 6 3 5 11 9 5 4 0 0 0 0 0 0 0 0 0 2 1 0 5 2 4 3 4 4 4 2 4 7 6 5 2 0 0
 1988 7 1 3 0 125 0 0 0 0 0 2 0 1 4 2 1 1 2 2 1 7 4 5 6 9 9 2 1 0 0 0 0 0 0 0 2 1 1 3 1 3 6 3 3 0 4 5 3 5 9 9 8 0 0 0
 1989 7 1 3 0 125 0 0 0 0 0 1 0 2 1 3 3 2 1 4 4 3 4 2 3 9 5 11 2 0 0 0 0 0 0 0 0 3 6 2 1 3 0 4 3 3 2 5 7 7 9 3 3 4 0 0
 1990 7 1 3 0 125 0 0 0 0 0 0 0 2 2 2 2 2 2 2 9 4 4 6 6 8 4 4 1 0 0 0 0 0 0 0 1 1 2 2 3 8 2 8 6 6 3 2 3 4 6 5 1 2 0 0
 1991 7 1 3 0 125 0 0 0 0 0 0 0 3 0 3 3 5 5 4 3 3 0 1 6 10 4 4 0 0 0 0 0 0 0 1 1 1 1 3 4 6 5 3 5 6 6 6 6 4 7 3 3 0 0 0
 1992 7 1 3 0 125 0 0 0 0 2 2 0 1 1 1 3 3 2 7 6 4 4 2 5 6 3 6 0 0 0 0 0 0 0 0 0 0 5 3 1 3 5 3 5 8 3 4 6 3 13 4 1 0 0 0
 1993 7 1 3 0 125 0 0 0 0 0 0 1 2 2 2 2 2 4 5 10 5 7 3 2 12 7 6 0 0 0 0 0 0 0 0 0 0 3 1 1 3 2 6 4 8 4 6 4 2 4 3 1 1 0 0
 1994 7 1 3 0 125 0 0 0 0 0 0 0 0 0 4 1 4 3 4 4 9 4 6 7 8 5 3 2 0 0 0 0 0 0 0 0 0 2 0 2 1 1 4 4 10 5 8 6 3 5 6 1 3 0 0
 1995 7 1 3 0 125 0 0 0 1 0 0 1 1 1 1 2 2 5 8 4 11 5 5 4 8 7 0 0 0 0 0 0 0 0 1 0 0 1 1 3 3 1 2 6 3 4 4 8 3 12 4 3 0 0 0
 1996 7 1 3 0 125 0 0 0 1 0 2 1 0 2 4 3 3 2 3 6 6 3 3 4 11 6 6 0 0 0 0 0 0 0 0 0 1 2 0 3 3 1 0 5 4 6 7 4 5 10 3 4 1 0 0
 1997 7 1 3 0 125 0 0 0 2 0 0 2 2 0 0 3 1 6 4 6 2 9 4 5 9 12 0 0 0 0 0 0 0 0 0 3 1 0 5 3 2 4 1 1 6 4 1 6 6 5 6 4 0 0 0
 1998 7 1 3 0 125 0 0 0 0 3 1 2 2 2 2 3 1 3 6 2 0 7 4 5 12 3 1 2 0 0 0 0 0 0 4 1 1 0 2 2 0 1 1 4 6 2 5 4 6 13 7 4 1 0 0
 1999 7 1 3 0 125 0 0 0 0 1 0 1 1 3 0 1 2 2 8 3 4 7 3 5 6 5 7 0 0 0 0 0 0 0 0 0 7 3 4 2 3 2 5 2 11 3 5 1 5 7 4 2 0 0 0
 2000 7 1 3 0 125 0 0 0 0 0 1 0 0 1 2 4 3 1 6 4 4 3 3 4 5 11 0 0 0 0 0 0 0 0 0 2 4 4 3 3 6 3 4 1 8 3 5 1 4 11 1 5 5 0 0
 2001 7 1 3 0 125 0 0 0 0 2 1 0 1 1 0 2 7 6 9 4 2 5 6 4 7 6 4 0 0 0 0 0 0 0 0 2 0 1 0 2 3 2 5 3 8 3 3 5 2 10 6 3 0 0 0
 1977 7 2 3 0 125 0 0 0 0 3 0 0 2 2 3 1 2 5 0 5 6 5 3 3 8 4 10 0 0 0 0 0 0 0 0 0 6 3 3 2 2 5 2 3 3 8 1 1 6 5 8 3 2 0 0
 1980 7 2 3 0 125 0 0 0 0 1 1 1 3 2 2 1 3 6 1 2 5 1 3 3 8 3 3 4 1 0 0 0 0 0 1 1 2 3 4 4 4 4 4 1 1 1 5 3 5 14 7 5 2 0 0
 1983 7 2 3 0 125 0 0 0 0 2 3 3 5 2 4 5 2 3 2 5 5 6 5 3 3 1 8 0 0 0 0 0 0 0 2 2 1 2 2 4 2 6 2 3 5 2 4 4 1 6 10 0 0 0 0
 1986 7 2 3 0 125 0 0 0 0 2 1 1 4 6 2 3 1 1 1 5 5 5 3 3 7 7 3 2 0 0 0 0 0 1 2 1 3 2 1 5 0 2 5 6 7 3 5 2 3 7 4 4 0 0 0
 1989 7 2 3 0 125 0 0 0 0 0 5 8 3 3 5 1 2 4 1 2 2 4 3 2 3 3 2 0 0 0 0 0 0 2 2 3 5 2 5 8 8 7 3 2 4 3 6 3 1 8 0 0 0 0 0
 1992 7 2 3 0 125 0 0 0 0 0 5 6 6 5 3 2 5 6 6 5 5 1 3 1 3 4 0 0 0 0 0 0 0 0 0 2 4 3 6 5 3 6 6 2 5 4 3 1 3 1 2 3 0 0 0
 1995 7 2 3 0 125 0 0 0 0 2 0 0 4 7 5 5 5 6 2 5 6 5 6 0 3 4 1 0 0 0 0 0 0 0 2 3 0 1 2 1 5 3 4 9 5 3 3 4 2 5 4 3 0 0 0
 1998 7 2 3 0 125 0 0 0 3 1 1 2 3 4 6 4 6 5 3 1 2 1 1 1 5 2 2 0 0 0 0 0 0 0 10 5 4 2 3 7 2 1 4 4 5 3 2 3 1 8 6 2 0 0 0
 2001 7 2 3 0 125 0 0 0 0 0 2 3 5 7 5 9 2 9 5 4 4 1 1 2 2 8 0 0 0 0 0 0 0 0 2 1 4 6 5 6 4 3 4 4 5 1 3 2 1 3 2 0 0 0 0
-9999 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
#
17 #_N_age_bins
 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 20 25
2 #_N_ageerror_definitions
 0.5 1.5 2.5 3.5 4.5 5.5 6.5 7.5 8.5 9.5 10.5 11.5 12.5 13.5 14.5 15.5 16.5 17.5 18.5 19.5 20.5 21.5 22.5 23.5 24.5 25.5 26.5 27.5 28.5 29.5 30.5 31.5 32.5 33.5 34.5 35.5 36.5 37.5 38.5 39.5 40.5
 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001
 0.5 1.5 2.5 3.5 4.5 5.5 6.5 7.5 8.5 9.5 10.5 11.5 12.5 13.5 14.5 15.5 16.5 17.5 18.5 19.5 20.5 21.5 22.5 23.5 24.5 25.5 26.5 27.5 28.5 29.5 30.5 31.5 32.5 33.5 34.5 35.5 36.5 37.5 38.5 39.5 40.5
 0.5 0.65 0.67 0.7 0.73 0.76 0.8 0.84 0.88 0.92 0.97 1.03 1.09 1.16 1.23 1.32 1.41 1.51 1.62 1.75 1.89 2.05 2.23 2.45 2.71 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3
#_mintailcomp: upper and lower distribution for females and males separately are accumulated until exceeding this level.
#_addtocomp:  after accumulation of tails; this value added to all bins
#_combM+F: males and females treated as combined gender below this bin number 
#_compressbins: accumulate upper tail by this number of bins; acts simultaneous with mintailcomp; set=0 for no forced accumulation
#_Comp_Error:  0=multinomial, 1=dirichlet
#_ParmSelect:  parm number for dirichlet
#_minsamplesize: minimum sample size; set to 1 to match 3.24, minimum value is 0.001
#
#_mintailcomp addtocomp combM+F CompressBins CompError ParmSelect minsamplesize
0 1e-07 1 0 0 0 1 #_fleet:1_FISHERY
0 1e-07 1 0 0 0 1 #_fleet:2_SURVEY1
0 1e-07 1 0 0 0 1 #_fleet:3_SURVEY2
1 #_Lbin_method_for_Age_Data: 1=poplenbins; 2=datalenbins; 3=lengths
# sex codes:  0=combined; 1=use female only; 2=use male only; 3=use both as joint sexxlength distribution
# partition codes:  (0=combined; 1=discard; 2=retained
#_yr month fleet sex part ageerr Lbin_lo Lbin_hi Nsamp datavector(female-male)
 1971 7 1 3 0 2 1 -1 75 0 0 0 0 3 1 1 4 2 1 0 1 2 2 13 2 3 0 0 4 2 1 1 2 1 2 2 1 2 1 2 6 5 8
 1972 7 1 3 0 2 1 -1 75 2 1 1 1 0 3 1 2 2 5 3 1 2 2 9 8 3 0 0 1 2 3 1 3 0 5 1 3 0 2 1 2 3 2
 1973 7 1 3 0 2 1 -1 75 0 0 1 0 1 1 2 3 3 1 1 5 2 2 7 4 3 0 0 0 4 1 3 5 1 2 3 1 3 2 0 5 3 6
 1974 7 1 3 0 2 1 -1 75 0 0 2 0 1 4 2 2 2 4 1 1 1 2 6 6 6 0 0 4 1 2 2 1 2 0 0 1 2 1 1 6 5 7
 1975 7 1 3 0 2 1 -1 75 0 0 1 2 3 1 1 1 2 1 2 2 2 3 10 3 4 0 0 0 0 10 1 2 3 2 1 0 0 0 0 9 3 6
 1976 7 1 3 0 2 1 -1 75 0 0 1 0 2 2 2 1 3 1 2 3 1 1 7 1 3 0 0 0 0 7 4 3 2 1 2 4 4 0 0 8 10 0
 1977 7 1 3 0 2 1 -1 75 0 0 0 0 7 1 0 0 2 4 2 2 3 1 7 2 3 0 0 2 1 4 2 3 3 4 2 2 2 0 1 8 3 4
 1978 7 1 3 0 2 1 -1 75 0 0 3 2 1 1 0 2 0 2 4 3 1 0 9 4 6 0 0 2 2 5 1 0 2 3 2 4 2 0 4 4 3 3
 1979 7 1 3 0 2 1 -1 75 2 0 1 5 2 1 2 3 3 3 2 2 1 0 3 7 0 0 0 2 0 1 0 2 3 2 5 1 3 1 2 6 9 1
 1980 7 1 3 0 2 1 -1 75 0 1 0 2 0 1 1 2 2 3 2 1 1 0 7 8 0 0 0 0 3 2 1 1 1 2 2 4 2 2 2 11 3 8
 1981 7 1 3 0 2 1 -1 75 0 4 0 3 7 2 2 2 2 1 1 2 2 1 4 4 6 0 0 3 2 2 1 1 3 2 2 0 1 2 2 5 3 3
 1982 7 1 3 0 2 1 -1 75 0 2 1 1 3 3 2 1 1 2 2 1 0 2 6 3 9 0 0 0 0 3 5 0 1 4 1 1 1 2 1 8 9 0
 1983 7 1 3 0 2 1 -1 75 0 0 0 6 1 2 2 2 1 1 4 5 0 0 6 2 7 0 0 3 1 3 5 1 0 1 1 3 0 3 3 5 3 4
 1984 7 1 3 0 2 1 -1 75 0 0 0 3 4 0 3 6 3 1 4 0 2 0 7 2 3 0 0 3 1 5 4 2 3 5 1 2 1 2 0 1 2 5
 1985 7 1 3 0 2 1 -1 75 0 0 0 5 1 2 4 5 0 2 4 3 2 3 3 4 5 0 0 0 1 2 3 2 4 2 0 2 3 1 1 7 2 2
 1986 7 1 3 0 2 1 -1 75 0 2 2 1 3 7 4 3 2 2 2 2 2 0 4 2 2 0 0 0 0 4 4 4 1 2 3 4 0 0 1 5 7 0
 1987 7 1 3 0 2 1 -1 75 0 3 1 3 1 2 3 4 2 3 3 2 2 1 3 2 0 0 0 7 1 5 1 4 2 4 3 2 3 1 0 2 1 4
 1988 7 1 3 0 2 1 -1 75 1 0 5 0 2 3 3 3 4 3 3 1 0 3 3 5 0 0 1 3 3 2 2 1 4 3 2 1 2 4 0 5 3 0
 1989 7 1 3 0 2 1 -1 75 0 3 1 1 4 3 7 1 5 1 1 4 1 0 1 7 0 0 0 5 3 4 1 1 5 3 1 5 2 1 0 2 2 0
 1990 7 1 3 0 2 1 -1 75 0 0 7 3 7 3 0 1 3 0 1 1 1 1 3 4 0 0 1 0 8 4 3 3 2 4 5 1 5 1 0 1 2 0
 1991 7 1 3 0 2 1 -1 75 0 0 4 1 7 4 2 3 2 1 0 1 1 3 3 3 0 0 3 4 2 5 4 4 1 3 3 0 4 2 0 4 1 0
 1992 7 1 3 0 2 1 -1 75 0 0 7 4 5 10 4 3 0 3 1 0 2 0 2 1 1 0 0 5 1 3 8 3 3 1 2 0 1 3 0 1 1 0
 1993 7 1 3 0 2 1 -1 75 0 0 7 4 3 7 5 7 2 1 0 1 0 4 0 0 0 0 0 3 3 4 3 7 0 0 4 2 1 1 1 5 0 0
 1994 7 1 3 0 2 1 -1 75 0 0 3 6 4 4 4 9 4 5 1 0 0 0 0 0 3 0 0 0 9 0 7 2 2 3 4 0 3 2 0 0 0 0
 1995 7 1 3 0 2 1 -1 75 3 1 2 0 8 5 2 6 2 5 0 2 1 4 0 0 0 0 0 0 2 5 3 2 3 5 6 1 0 1 1 3 1 1
 1996 7 1 3 0 2 1 -1 75 0 0 1 1 5 4 3 7 2 3 2 3 3 1 5 1 0 0 2 5 0 5 4 1 2 3 4 2 3 0 1 2 0 0
 1997 7 1 3 0 2 1 -1 75 0 5 3 5 0 2 4 3 4 5 1 1 3 2 2 0 0 0 0 0 3 1 6 5 5 2 3 4 1 2 3 0 0 0
 1998 7 1 3 0 2 1 -1 75 5 3 1 4 1 2 3 4 3 2 0 2 0 1 5 0 0 0 0 4 6 4 2 7 2 1 1 6 3 0 0 2 1 0
 1999 7 1 3 0 2 1 -1 75 2 2 3 3 6 3 3 3 8 3 3 3 0 1 1 0 0 0 1 3 3 3 5 4 0 4 2 4 0 1 0 1 0 0
 2000 7 1 3 0 2 1 -1 75 0 2 1 9 4 4 2 2 4 3 1 0 1 0 5 0 0 0 0 8 11 3 1 2 2 1 1 2 1 0 2 3 0 0
 2001 7 1 3 0 2 1 -1 75 0 1 1 6 8 1 1 0 5 2 2 2 0 3 4 0 0 0 0 5 3 4 6 3 3 1 4 3 1 1 2 3 0 0
 1977 7 2 3 0 2 1 -1 75 2 1 2 1 0 4 3 3 2 1 1 0 1 1 4 7 0 0 2 2 7 1 0 1 0 1 2 4 1 2 2 7 10 0
 1980 7 2 3 0 2 1 -1 75 3 3 4 6 5 2 0 2 3 0 3 2 2 2 2 1 4 0 2 3 5 3 1 2 1 1 2 1 1 1 0 3 1 4
 1983 7 2 3 0 2 1 -1 75 3 4 3 2 3 0 0 7 0 0 3 1 1 0 5 6 0 0 2 2 4 1 2 3 4 3 2 0 1 1 2 7 1 2
 1986 7 2 3 0 2 1 -1 75 3 0 2 5 3 5 5 3 1 3 2 1 1 1 3 0 2 0 0 2 3 6 6 1 3 3 1 1 1 1 2 2 3 0
 1989 7 2 3 0 2 1 -1 75 7 3 7 3 2 1 0 3 2 1 2 1 1 5 0 0 0 0 4 8 6 1 2 3 5 1 1 2 0 4 0 0 0 0
 1992 7 2 3 0 2 1 -1 75 2 5 3 4 0 5 0 5 2 0 0 0 1 0 3 0 0 0 4 5 5 10 8 6 2 1 2 0 0 1 0 1 0 0
 1995 7 2 3 0 2 1 -1 75 0 5 2 3 2 3 5 4 2 1 1 2 0 0 3 0 0 0 2 3 5 11 2 6 5 1 2 1 2 0 0 2 0 0
 1998 7 2 3 0 2 1 -1 75 9 4 4 3 1 1 1 1 3 3 1 2 1 7 0 0 0 0 6 5 3 5 1 3 3 2 3 2 0 1 0 0 0 0
 2001 7 2 3 0 2 1 -1 75 4 0 4 11 5 3 4 2 2 0 0 0 0 0 2 0 0 0 2 4 7 11 5 2 0 2 2 2 0 0 0 1 0 0
-9999  0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
#
1 #_Use_MeanSize-at-Age_obs (0/1)
# sex codes:  0=combined; 1=use female only; 2=use male only; 3=use both as joint sexxlength distribution
# partition codes:  (0=combined; 1=discard; 2=retained
# ageerr codes:  positive means mean length-at-age; negative means mean bodywt_at_age
#_yr month fleet sex part ageerr ignore datavector(female-male)
#                                          samplesize(female-male)
 1971 7 1 3 0 1 2 29.8931 40.6872 44.7411 50.027 52.5794 56.1489 57.1033 61.1728 61.7417 63.368 64.4088 65.6889 67.616 68.5972 69.9177 71.0443 72.3609 32.8188 39.5964 43.988 50.1693 53.1729 54.9822 55.3463 60.3509 60.7439 62.3432 64.3224 65.1032 64.1965 66.7452 67.5154 70.8749 71.2768 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20
 1995 7 1 3 0 1 2 32.8974 38.2709 43.8878 49.2745 53.5343 55.1978 57.4389 62.0368 62.1445 62.9579 65.0857 65.6433 66.082 65.6117 67.0784 69.3493 72.2966 32.6552 40.5546 44.6292 50.4063 52.0796 56.1529 56.9004 60.218 61.5894 63.6613 64.0222 63.4926 65.8115 69.5357 68.2448 66.881 71.5122 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20
 1971 7 2 3 0 1 2 34.1574 38.8017 43.122 47.2042 49.0502 51.6446 56.3201 56.3038 60.5509 60.2537 59.8042 62.9309 66.842 67.8089 71.1612 70.7693 74.5593 35.3811 40.7375 44.5192 47.6261 52.5298 53.5552 54.9851 58.9231 58.9932 61.8625 64.0366 62.7507 63.9754 64.5102 66.9779 67.7361 69.1298 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20
 1995 7 2 3 0 1 2 34.6022 38.3176 42.9052 48.2752 50.6189 53.476 56.7806 59.4127 60.5964 60.5537 65.3608 64.7263 67.4315 67.1405 68.9908 71.9886 74.1594 35.169 40.2404 43.8878 47.3519 49.9906 52.2207 54.9035 58.6058 60.0957 62.4046 62.2298 62.1437 66.2116 65.7657 69.9544 70.6518 71.4371 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20
-9999  0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
#
0 #_N_environ_variables
# -2 in yr will subtract mean for that env_var; -1 will subtract mean and divide by stddev (e.g. Z-score)
#Yr Variable Value
#
0 # N sizefreq methods to read 
#
0 # do tags (0/1)
#
0 #    morphcomp data(0/1) 
#  Nobs, Nmorphs, mincomp
#  yr, seas, type, partition, Nsamp, datavector_by_Nmorphs
#
0  #  Do dataread for selectivity priors(0/1)
# Yr, Seas, Fleet,  Age/Size,  Bin,  selex_prior,  prior_sd
# feature not yet implemented
#
999

ENDDATA


/////////////////////////////////////////////////////////////
////// TwoFisheries model
//////////////////////////////////////////////////////////////
// only the Control and Data ss files are needed to re-run the results
/////// Control.ss
#V3.30.17.00;_2021_06_11;_safe;_Stock_Synthesis_by_Richard_Methot_(NOAA)_using_ADMB_12.3
#Stock Synthesis (SS) is a work of the U.S. Government and is not subject to copyright protection in the United States.
#Foreign copyrights may apply. See copyright.txt for more information.
#_user_support_available_at:NMFS.Stock.Synthesis@noaa.gov
#_user_info_available_at:https://vlab.noaa.gov/group/stock-synthesis
#C growth parameters are estimated
#C spawner-recruitment bias adjustment Not tuned For optimality
#_data_and_control_files: data.ss // control.ss
0  # 0 means do not read wtatage.ss; 1 means read and use wtatage.ss and also read and use growth parameters
1  #_N_Growth_Patterns (Growth Patterns, Morphs, Bio Patterns, GP are terms used interchangeably in SS)
1 #_N_platoons_Within_GrowthPattern 
#_Cond 1 #_Platoon_within/between_stdev_ratio (no read if N_platoons=1)
#_Cond  1 #vector_platoon_dist_(-1_in_first_val_gives_normal_approx)
#
2 # recr_dist_method for parameters:  2=main effects for GP, Area, Settle timing; 3=each Settle entity; 4=none (only when N_GP*Nsettle*pop==1)
1 # not yet implemented; Future usage: Spawner-Recruitment: 1=global; 2=by area
1 #  number of recruitment settlement assignments 
0 # unused option
#GPattern month  area  age (for each settlement assignment)
 1 1 1 0
#
#_Cond 0 # N_movement_definitions goes here if Nareas > 1
#_Cond 1.0 # first age that moves (real age at begin of season, not integer) also cond on do_migration>0
#_Cond 1 1 1 2 4 10 # example move definition for seas=1, morph=1, source=1 dest=2, age1=4, age2=10
#
1 #_Nblock_Patterns
 1 #_blocks_per_pattern 
# begin and end years of blocks
 1970 1970
#
# controls for all timevary parameters 
1 #_time-vary parm bound check (1=warn relative to base parm bounds; 3=no bound check); Also see env (3) and dev (5) options to constrain with base bounds
#
# AUTOGEN
 0 0 0 0 0 # autogen: 1st element for biology, 2nd for SR, 3rd for Q, 4th reserved, 5th for selex
# where: 0 = autogen time-varying parms of this category; 1 = read each time-varying parm line; 2 = read then autogen if parm min==-12345
#
#_Available timevary codes
#_Block types: 0: P_block=P_base*exp(TVP); 1: P_block=P_base+TVP; 2: P_block=TVP; 3: P_block=P_block(-1) + TVP
#_Block_trends: -1: trend bounded by base parm min-max and parms in transformed units (beware); -2: endtrend and infl_year direct values; -3: end and infl as fraction of base range
#_EnvLinks:  1: P(y)=P_base*exp(TVP*env(y));  2: P(y)=P_base+TVP*env(y);  3: P(y)=f(TVP,env_Zscore) w/ logit to stay in min-max;  4: P(y)=2.0/(1.0+exp(-TVP1*env(y) - TVP2))
#_DevLinks:  1: P(y)*=exp(dev(y)*dev_se;  2: P(y)+=dev(y)*dev_se;  3: random walk;  4: zero-reverting random walk with rho;  5: like 4 with logit transform to stay in base min-max
#_DevLinks(more):  21-25 keep last dev for rest of years
#
#_Prior_codes:  0=none; 6=normal; 1=symmetric beta; 2=CASAL's beta; 3=lognormal; 4=lognormal with biascorr; 5=gamma
#
# setup for M, growth, wt-len, maturity, fecundity, (hermaphro), recr_distr, cohort_grow, (movement), (age error), (catch_mult), sex ratio 
#_NATMORT
0 #_natM_type:_0=1Parm; 1=N_breakpoints;_2=Lorenzen;_3=agespecific;_4=agespec_withseasinterpolate;_5=BETA:_Maunder_link_to_maturity
  #_no additional input for selected M option; read 1P per morph
#
1 # GrowthModel: 1=vonBert with L1&L2; 2=Richards with L1&L2; 3=age_specific_K_incr; 4=age_specific_K_decr; 5=age_specific_K_each; 6=NA; 7=NA; 8=growth cessation
0 #_Age(post-settlement)_for_L1;linear growth below this
999 #_Growth_Age_for_L2 (999 to use as Linf)
-999 #_exponential decay for growth above maxage (value should approx initial Z; -999 replicates 3.24; -998 to not allow growth above maxage)
0  #_placeholder for future growth feature
#
0 #_SD_add_to_LAA (set to 0.1 for SS2 V1.x compatibility)
0 #_CV_Growth_Pattern:  0 CV=f(LAA); 1 CV=F(A); 2 SD=F(LAA); 3 SD=F(A); 4 logSD=F(A)
#
2 #_maturity_option:  1=length logistic; 2=age logistic; 3=read age-maturity matrix by growth_pattern; 4=read age-fecundity; 5=disabled; 6=read length-maturity
1 #_First_Mature_Age
1 #_fecundity option:(1)eggs=Wt*(a+b*Wt);(2)eggs=a*L^b;(3)eggs=a*Wt^b; (4)eggs=a+b*L; (5)eggs=a+b*W
0 #_hermaphroditism option:  0=none; 1=female-to-male age-specific fxn; -1=male-to-female age-specific fxn
1 #_parameter_offset_approach for M, G, CV_G:  1- direct, no offset**; 2- male=fem_parm*exp(male_parm); 3: male=female*exp(parm) then old=young*exp(parm)
#_** in option 1, any male parameter with value = 0.0 and phase <0 is set equal to female parameter
#
#_growth_parms
#_ LO HI INIT PRIOR PR_SD PR_type PHASE env_var&link dev_link dev_minyr dev_maxyr dev_PH Block Block_Fxn
# Sex: 1  BioPattern: 1  NatMort
 0.05 0.15 0.15 0.1 0.8 0 -3 0 0 0 0 0 0 0 # NatM_uniform_Fem_GP_1
# Sex: 1  BioPattern: 1  Growth
 -10 45 0 36 10 6 2 0 0 0 0 0 0 0 # L_at_Amin_Fem_GP_1
 40 90 68 70 10 6 4 0 0 0 0 0 0 0 # L_at_Amax_Fem_GP_1
 0.05 0.25 0.167 0.15 0.8 6 4 0 0 0 0 0 0 0 # VonBert_K_Fem_GP_1
 0.05 0.25 0.05 0.1 0.8 0 -3 0 0 0 0 0 0 0 # CV_young_Fem_GP_1
 0.05 0.25 0.05 0.1 0.8 0 -3 0 0 0 0 0 0 0 # CV_old_Fem_GP_1
# Sex: 1  BioPattern: 1  WtLen
 -3 3 2.44e-06 2.44e-06 0.8 0 -3 0 0 0 0 0 0 0 # Wtlen_1_Fem_GP_1
 -3 4 3.34694 3.34694 0.8 0 -3 0 0 0 0 0 0 0 # Wtlen_2_Fem_GP_1
# Sex: 1  BioPattern: 1  Maturity&Fecundity
0 60 5 55 0.8 0 -3 0 0 0 0 0 0 0 # Mat50%_Fem_GP_1
 -3 3 -0.99 -0.25 0.8 0 -3 0 0 0 0 0 0 0 # Mat_slope_Fem_GP_1
 -3 3 1 1 0.8 0 -3 0 0 0 0 0 0 0 # Eggs/kg_inter_Fem_GP_1
 -3 3 0 0 0.8 0 -3 0 0 0 0 0 0 0 # Eggs/kg_slope_wt_Fem_GP_1
# Sex: 2  BioPattern: 1  NatMort
 0.05 0.15 0.15 0.1 0.8 0 -3 0 0 0 0 0 0 0 # NatM_uniform_Mal_GP_1
# Sex: 2  BioPattern: 1  Growth
 0 45 0 36 10 0 -3 0 0 0 0 0 0 0 # L_at_Amin_Mal_GP_1
 40 90 68 70 10 6 4 0 0 0 0 0 0 0 # L_at_Amax_Mal_GP_1
 0.05 0.25 0.167 0.15 0.8 6 4 0 0 0 0 0 0 0 # VonBert_K_Mal_GP_1
 0.05 0.25 0.05 0.1 0.8 0 -3 0 0 0 0 0 0 0 # CV_young_Mal_GP_1
 0.05 0.25 0.05 0.1 0.8 0 -3 0 0 0 0 0 0 0 # CV_old_Mal_GP_1
# Sex: 2  BioPattern: 1  WtLen
 -3 3 2.44e-06 2.44e-06 0.8 0 -3 0 0 0 0 0 0 0 # Wtlen_1_Mal_GP_1
 -3 4 3.34694 3.34694 0.8 0 -3 0 0 0 0 0 0 0 # Wtlen_2_Mal_GP_1
# Hermaphroditism
#  Recruitment Distribution  
 0 0 0 0 0 0 -4 0 0 0 0 0 0 0 # RecrDist_GP_1
 0 0 0 0 0 0 -4 0 0 0 0 0 0 0 # RecrDist_Area_1
 0 0 0 0 0 0 -4 0 0 0 0 0 0 0 # RecrDist_month_1
#  Cohort growth dev base
 0.1 10 1 1 1 0 -1 0 0 0 0 0 0 0 # CohortGrowDev
#  Movement
#  Age Error from parameters
#  catch multiplier
#  fraction female, by GP
 1e-06 0.999999 0.5 0.5 0.5 0 -99 0 0 0 0 0 0 0 # FracFemale_GP_1
#
#_no timevary MG parameters
#
#_seasonal_effects_on_biology_parms
 0 0 0 0 0 0 0 0 0 0 #_femwtlen1,femwtlen2,mat1,mat2,fec1,fec2,Malewtlen1,malewtlen2,L1,K
#_ LO HI INIT PRIOR PR_SD PR_type PHASE
#_Cond -2 2 0 0 -1 99 -2 #_placeholder when no seasonal MG parameters
#
3 #_Spawner-Recruitment; Options: 1=NA; 2=Ricker; 3=std_B-H; 4=SCAA; 5=Hockey; 6=B-H_flattop; 7=survival_3Parm; 8=Shepherd_3Parm; 9=RickerPower_3parm
0  # 0/1 to use steepness in initial equ recruitment calculation
0  #  future feature:  0/1 to make realized sigmaR a function of SR curvature
#_          LO            HI          INIT         PRIOR         PR_SD       PR_type      PHASE    env-var    use_dev   dev_mnyr   dev_mxyr     dev_PH      Block    Blk_Fxn #  parm_name
             3            31       10.6          10.3            10             0          1          0          0          0          0          0          0          0 # SR_LN(R0)
           0.2             1      		1           0.7          0.05             1          4          0          0          0          0          0          0          0 # SR_BH_steep
             0             2           0.6           0.8           0.8             0         -4          0          0          0          0          0          0          0 # SR_sigmaR
            -5             5             0             0             1             0         -4          0          0          0          0          0          0          0 # SR_regime
             0             0             0             0             0             0        -99          0          0          0          0          0          0          0 # SR_autocorr
#_no timevary SR parameters
1 #do_recdev:  0=none; 1=devvector (R=F(SSB)+dev); 2=deviations (R=F(SSB)+dev); 3=deviations (R=R0*dev; dev2=R-f(SSB)); 4=like 3 with sum(dev2) adding penalty
1971 # first year of main recr_devs; early devs can preceed this era
2001 # last year of main recr_devs; forecast devs start in following year
2 #_recdev phase 
1 # (0/1) to read 13 advanced options
 0 #_recdev_early_start (0=none; neg value makes relative to recdev_start)
 -4 #_recdev_early_phase
 0 #_forecast_recruitment phase (incl. late recr) (0 value resets to maxphase+1)
 1 #_lambda for Fcast_recr_like occurring before endyr+1
 1900 #_last_yr_nobias_adj_in_MPD; begin of ramp
 1900 #_first_yr_fullbias_adj_in_MPD; begin of plateau
 2001 #_last_yr_fullbias_adj_in_MPD
 2002 #_end_yr_for_ramp_in_MPD (can be in forecast to shape ramp, but SS sets bias_adj to 0.0 for fcast yrs)
 0 #_max_bias_adj_in_MPD (typical ~0.8; -3 sets all years to 0.0; -2 sets all non-forecast yrs w/ estimated recdevs to 1.0; -1 sets biasadj=1.0 for all yrs w/ recdevs)
 0 #_period of cycles in recruitment (N parms read below)
 -5 #min rec_dev
 5 #max rec_dev
 0 #_read_recdevs
#_end of advanced SR options
#
#_placeholder for full parameter lines for recruitment cycles
# read specified recr devs
#_Yr Input_value
#
# all recruitment deviations
#  1971R 1972R 1973R 1974R 1975R 1976R 1977R 1978R 1979R 1980R 1981R 1982R 1983R 1984R 1985R 1986R 1987R 1988R 1989R 1990R 1991R 1992R 1993R 1994R 1995R 1996R 1997R 1998R 1999R 2000R 2001R 2002F 2003F 2004F 2005F 2006F 2007F 2008F 2009F 2010F 2011F
#  0.1268 -0.0629442 0.0998014 -0.174095 0.0306484 0.714818 -0.0228752 0.00379775 0.261267 0.173626 0.0900049 -0.226622 -0.439888 -0.312088 0.393112 0.551725 0.21891 0.154932 -0.384786 0.596744 -0.682432 -0.273424 -0.829665 0.365024 -0.605267 0.455103 1.11072 -0.546499 -0.656469 0.171606 -0.301581 0 0 0 0 0 0 0 0 0 0
#
#Fishing Mortality info 
0.3 # F ballpark value in units of annual_F
-2001 # F ballpark year (neg value to disable)
3 # F_Method:  1=Pope; 2=instan. F; 3=hybrid (hybrid is recommended)
2.95 # max F or harvest rate, depends on F_Method
# no additional F input needed for Fmethod 1
# if Fmethod=2; read overall start F value; overall phase; N detailed inputs to read
# if Fmethod=3; read N iterations for tuning for Fmethod 3
4  # N iterations for tuning F in hybrid method (recommend 3 to 7)
#
#_initial_F_parms; for each fleet x season that has init_catch; nest season in fleet; count = 0
#_for unconstrained init_F, use an arbitrary initial catch and set lambda=0 for its logL
#_ LO HI INIT PRIOR PR_SD  PR_type  PHASE
#
# F rates by fleet x season
# Yr:  1971 1972 1973 1974 1975 1976 1977 1978 1979 1980 1981 1982 1983 1984 1985 1986 1987 1988 1989 1990 1991 1992 1993 1994 1995 1996 1997 1998 1999 2000 2001 2002 2003 2004 2005 2006 2007 2008 2009 2010 2011
# seas:  1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
# FISHERY 0 0.00211081 0.010609 0.0107037 0.0217063 0.0333334 0.0459509 0.0599453 0.0757167 0.107737 0.146876 0.162531 0.180868 0.202893 0.230365 0.266192 0.314644 0.338215 0.354481 0.356016 0.338877 0.238035 0.242891 0.250688 0.26355 0.283377 0.227156 0.238194 0.247552 0.252337 0.253174 0.0129829 0.0279253 0.038022 0.0447387 0.0493313 0.0527091 0.0554663 0.0579281 0.0602317 0.0624094
#
#_Q_setup for fleets with cpue or survey data
#_1:  fleet number
#_2:  link type: (1=simple q, 1 parm; 2=mirror simple q, 1 mirrored parm; 3=q and power, 2 parm; 4=mirror with offset, 2 parm)
#_3:  extra input for link, i.e. mirror fleet# or dev index number
#_4:  0/1 to select extra sd parameter
#_5:  0/1 for biasadj or not
#_6:  0/1 to float
#_   fleet      link link_info  extra_se   biasadj     float  #  fleetname
         3         1         0         1         0         0  #  SURVEY1
         4         1         0         0         0         0  #  SURVEY2
-9999 0 0 0 0 0
#
#_Q_parms(if_any);Qunits_are_ln(q)
#_          LO            HI          INIT         PRIOR         PR_SD       PR_type      PHASE    env-var    use_dev   dev_mnyr   dev_mxyr     dev_PH      Block    Blk_Fxn  #  parm_name
            -7             5      0.516018             0             1             0          1          0          0          0          0          0          0          0  #  LnQ_base_SURVEY1(2)
             0           0.5             0          0.05             1             0         -4          0          0          0          0          0          0          0  #  Q_extraSD_SURVEY1(2)
            -7             5       -6.6281             0             1             0          1          0          0          0          0          0          0          0  #  LnQ_base_SURVEY2(3)
#_no timevary Q parameters
#
#_size_selex_patterns
#Pattern:_0;  parm=0; selex=1.0 for all sizes
#Pattern:_1;  parm=2; logistic; with 95% width specification
#Pattern:_2;  parm=6; modification of pattern 24 with improved sex-specific offset
#Pattern:_5;  parm=2; mirror another size selex; PARMS pick the min-max bin to mirror
#Pattern:_11; parm=2; selex=1.0  for specified min-max population length bin range
#Pattern:_15; parm=0; mirror another age or length selex
#Pattern:_6;  parm=2+special; non-parm len selex
#Pattern:_43; parm=2+special+2;  like 6, with 2 additional param for scaling (average over bin range)
#Pattern:_8;  parm=8; double_logistic with smooth transitions and constant above Linf option
#Pattern:_9;  parm=6; simple 4-parm double logistic with starting length; parm 5 is first length; parm 6=1 does desc as offset
#Pattern:_21; parm=2+special; non-parm len selex, read as pairs of size, then selex
#Pattern:_22; parm=4; double_normal as in CASAL
#Pattern:_23; parm=6; double_normal where final value is directly equal to sp(6) so can be >1.0
#Pattern:_24; parm=6; double_normal with sel(minL) and sel(maxL), using joiners
#Pattern:_25; parm=3; exponential-logistic in length
#Pattern:_27; parm=special+3; cubic spline in length; parm1==1 resets knots; parm1==2 resets all 
#Pattern:_42; parm=special+3+2; cubic spline; like 27, with 2 additional param for scaling (average over bin range)
#_discard_options:_0=none;_1=define_retention;_2=retention&mortality;_3=all_discarded_dead;_4=define_dome-shaped_retention
#_Pattern Discard Male Special
 0 0 0 0 # 1 FISHERY
 0 0 0 0 # 2 FISHERY
 1 0 0 0 # 3 SURVEY1
 0 0 0 0 # 4 SURVEY2
#
#_age_selex_patterns
#Pattern:_0; parm=0; selex=1.0 for ages 0 to maxage
#Pattern:_10; parm=0; selex=1.0 for ages 1 to maxage
#Pattern:_11; parm=2; selex=1.0  for specified min-max age
#Pattern:_12; parm=2; age logistic
#Pattern:_13; parm=8; age double logistic
#Pattern:_14; parm=nages+1; age empirical
#Pattern:_15; parm=0; mirror another age or length selex
#Pattern:_16; parm=2; Coleraine - Gaussian
#Pattern:_17; parm=nages+1; empirical as random walk  N parameters to read can be overridden by setting special to non-zero
#Pattern:_41; parm=2+nages+1; // like 17, with 2 additional param for scaling (average over bin range)
#Pattern:_18; parm=8; double logistic - smooth transition
#Pattern:_19; parm=6; simple 4-parm double logistic with starting age
#Pattern:_20; parm=6; double_normal,using joiners
#Pattern:_26; parm=3; exponential-logistic in age
#Pattern:_27; parm=3+special; cubic spline in age; parm1==1 resets knots; parm1==2 resets all 
#Pattern:_42; parm=2+special+3; // cubic spline; with 2 additional param for scaling (average over bin range)
#Age patterns entered with value >100 create Min_selage from first digit and pattern from remainder
#_Pattern Discard Male Special
 12 0 0 0 # 1 FISHERY
 12 0 0 0 # 2 FISHERY
 11 0 0 0 # 3 SURVEY1
 11 0 0 0 # 4 SURVEY2
#
#_          LO            HI          INIT         PRIOR         PR_SD       PR_type      PHASE    env-var    use_dev   dev_mnyr   dev_mxyr     dev_PH      Block    Blk_Fxn  #  parm_name
# 1   FISHERY LenSelex
#            19            80       53.6411            50          0.01             1          2          0          0          0          0          0          0          0  #  Size_inflection_FISHERY(1)
#         0.01            60       18.9232            15          0.01             1          3          0          0          0          0          0          0          0  #  Size_95%width_FISHERY(1)
# 2   SURVEY1 LenSelex
            19            70        36.653            30          0.01             1          2          0          0          0          0          0          0          0  #  Size_inflection_SURVEY1(2)
          0.01            60       6.59179            10          0.01             1          3          0          0          0          0          0          0          0  #  Size_95%width_SURVEY1(2)
# 3   SURVEY2 LenSelex
# 1   FISHERY AgeSelex
             0            40             4             5            99             0        3         0          0          0          0          0          0          0  #  minage@sel=1_FISHERY(1)
             0            40            3             6            99             0        3          0          0          0          0          0          0          0  #  maxage@sel=1_FISHERY(1)
# 1   FISHERY AgeSelex
             0            40             5             5            99             0        3         0          0          0          0          0          0          0  #  minage@sel=1_FISHERY(1)
             0            40            3.5             6            99             0        3          0          0          0          0          0          0          0  #  maxage@sel=1_FISHERY(1)
# 2   SURVEY1 AgeSelex
             0            40             0             5            99             0        -99          0          0          0          0          0          0          0  #  minage@sel=1_SURVEY1(2)
             0            40            40             6            99             0        -99          0          0          0          0          0          0          0  #  maxage@sel=1_SURVEY1(2)
# 3   SURVEY2 AgeSelex
             0            40             0             5            99             0        -99          0          0          0          0          0          0          0  #  minage@sel=1_SURVEY2(3)
             0            40             0             6            99             0        -99          0          0          0          0          0          0          0  #  maxage@sel=1_SURVEY2(3)
#_No_Dirichlet parameters
#_no timevary selex parameters
#
0   #  use 2D_AR1 selectivity(0/1)
#_no 2D_AR1 selex offset used
#
# Tag loss and Tag reporting parameters go next
0  # TG_custom:  0=no read and autogen if tag data exist; 1=read
#_Cond -6 6 1 1 2 0.01 -4 0 0 0 0 0 0 0  #_placeholder if no parameters
#
# no timevary parameters
#
#
# Input variance adjustments factors: 
 #_1=add_to_survey_CV
 #_2=add_to_discard_stddev
 #_3=add_to_bodywt_CV
 #_4=mult_by_lencomp_N
 #_5=mult_by_agecomp_N
 #_6=mult_by_size-at-age_N
 #_7=mult_by_generalized_sizecomp
#_Factor  Fleet  Value
 -9999   1    0  # terminator
#
4 #_maxlambdaphase
1 #_sd_offset; must be 1 if any growthCV, sigmaR, or survey extraSD is an estimated parameter
# read 3 changes to default Lambdas (default value is 1.0)
# Like_comp codes:  1=surv; 2=disc; 3=mnwt; 4=length; 5=age; 6=SizeFreq; 7=sizeage; 8=catch; 9=init_equ_catch; 
# 10=recrdev; 11=parm_prior; 12=parm_dev; 13=CrashPen; 14=Morphcomp; 15=Tag-comp; 16=Tag-negbin; 17=F_ballpark; 18=initEQregime
#like_comp fleet  phase  value  sizefreq_method
 1 2 2 1 1
 4 2 2 1 1
 4 2 3 1 1
-9999  1  1  1  1  #  terminator
#
# lambdas (for info only; columns are phases)
#  0 0 0 0 #_CPUE/survey:_1
#  1 1 1 1 #_CPUE/survey:_2
#  1 1 1 1 #_CPUE/survey:_3
#  1 1 1 1 #_lencomp:_1
#  1 1 1 1 #_lencomp:_2
#  0 0 0 0 #_lencomp:_3
#  1 1 1 1 #_agecomp:_1
#  1 1 1 1 #_agecomp:_2
#  0 0 0 0 #_agecomp:_3
#  1 1 1 1 #_size-age:_1
#  1 1 1 1 #_size-age:_2
#  0 0 0 0 #_size-age:_3
#  1 1 1 1 #_init_equ_catch1
#  1 1 1 1 #_init_equ_catch2
#  1 1 1 1 #_init_equ_catch3
#  1 1 1 1 #_recruitments
#  1 1 1 1 #_parameter-priors
#  1 1 1 1 #_parameter-dev-vectors
#  1 1 1 1 #_crashPenLambda
#  0 0 0 0 # F_ballpark_lambda
1 # (0/1/2) read specs for more stddev reporting: 0 = skip, 1 = read specs for reporting stdev for selectivity, size, and numbers, 2 = add options for M,Dyn. Bzero, SmryBio
 1 1 -1 5 # Selectivity: (1) 0 to skip or fleet, (2) 1=len/2=age/3=combined, (3) year, (4) N selex bins; NOTE: combined reports in age bins
 1 5 # Growth: (1) 0 to skip or growth pattern, (2) growth ages; NOTE: does each sex
 1 -1 5 # Numbers-at-age: (1) 0 or area(-1 for all), (2) year, (3) N ages;  NOTE: sums across morphs
 5 15 25 35 43 # vector with selex std bins (-1 in first bin to self-generate)
 1 2 14 26 40 # vector with growth std ages picks (-1 in first bin to self-generate)
 1 2 14 26 40 # vector with NatAge std ages (-1 in first bin to self-generate)
999

/////// data.ss
#V3.30.17.00;_2021_06_11;_safe;_Stock_Synthesis_by_Richard_Methot_(NOAA)_using_ADMB_12.3
#Stock Synthesis (SS) is a work of the U.S. Government and is not subject to copyright protection in the United States.
#Foreign copyrights may apply. See copyright.txt for more information.
#_user_support_available_at:NMFS.Stock.Synthesis@noaa.gov
#_user_info_available_at:https://vlab.noaa.gov/group/stock-synthesis
#_Start_time: Fri Jun 11 08:38:55 2021
#_Number_of_datafiles: 1
#C data file for simple example
#_observed data: 
#V3.30.17.00;_2021_06_11;_safe;_Stock_Synthesis_by_Richard_Methot_(NOAA)_using_ADMB_12.3
#Stock Synthesis (SS) is a work of the U.S. Government and is not subject to copyright protection in the United States.
#Foreign copyrights may apply. See copyright.txt for more information.
1971 #_StartYr
2001 #_EndYr
1 #_Nseas
12 #_months/season
2 #_Nsubseasons (even number, minimum is 2)
0 #_spawn_month
2 #_Ngenders: 1, 2, -1  (use -1 for 1 sex setup with SSB multiplied by female_frac parameter)
40 #_Nages=accumulator age, first age is always age 0
1 #_Nareas
4 #_Nfleets (including surveys)
#_fleet_type: 1=catch fleet; 2=bycatch only fleet; 3=survey; 4=ignore 
#_sample_timing: -1 for fishing fleet to use season-long catch-at-age for observations, or 1 to use observation month;  (always 1 for surveys)
#_fleet_area:  area the fleet/survey operates in 
#_units of catch:  1=bio; 2=num (ignored for surveys; their units read later)
#_catch_mult: 0=no; 1=yes
#_rows are fleets
#_fleet_type fishery_timing area catch_units need_catch_mult fleetname
 1 -1 1 1 0 FISHERY1  # 1
 1 -1 1 1 0 FISHERY2  # 2
 3 1 1 2 0 SURVEY1  # 3
 3 1 1 2 0 SURVEY2  # 4
#Bycatch_fleet_input_goes_next
#a:  fleet index
#b:  1=include dead bycatch in total dead catch for F0.1 and MSY optimizations and forecast ABC; 2=omit from total catch for these purposes (but still include the mortality)
#c:  1=Fmult scales with other fleets; 2=bycatch F constant at input value; 3=bycatch F from range of years
#d:  F or first year of range
#e:  last year of range
#f:  not used
# a   b   c   d   e   f 
#_Catch data: yr, seas, fleet, catch, catch_se
#_catch_se:  standard error of log(catch)
#_NOTE:  catch data is ignored for survey fleets
-999	1	1	0	0.01
1971	1	1	0	0.01
1972	1	1	200	0.01
1973	1	1	1000	0.01
1974	1	1	1000	0.01
1975	1	1	2000	0.01
1976	1	1	3000	0.01
1977	1	1	4000	0.01
1978	1	1	5000	0.01
1979	1	1	6000	0.01
1980	1	1	8000	0.01
1981	1	1	10000	0.01
1982	1	1	10000	0.01
1983	1	1	10000	0.01
1984	1	1	10000	0.01
1985	1	1	10000	0.01
1986	1	1	10000	0.01
1987	1	1	10000	0.01
1988	1	1	9000	0.01
1989	1	1	8000	0.01
1990	1	1	7000	0.01
1991	1	1	6000	0.01
1992	1	1	4000	0.01
1993	1	1	4000	0.01
1994	1	1	4000	0.01
1995	1	1	4000	0.01
1996	1	1	4000	0.01
1997	1	1	3000	0.01
1998	1	1	3000	0.01
1999	1	1	3000	0.01
2000	1	1	3000	0.01
2001	1	1	3000	0.01
1971	1	2	0		0.01
1972	1	2	100		0.01
1973	1	2	500		0.01
1974	1	2	500		0.01
1975	1	2	1000	0.01
1976	1	2	1500	0.01
1977	1	2	2000	0.01
1978	1	2	2500	0.01
1979	1	2	3000	0.01
1980	1	2	4000	0.01
1981	1	2	5000	0.01
1982	1	2	5000	0.01
1983	1	2	5000	0.01
1984	1	2	5000	0.01
1985	1	2	5000	0.01
1986	1	2	5000	0.01
1987	1	2	5000	0.01
1988	1	2	4500	0.01
1989	1	2	4000	0.01
1990	1	2	3500	0.01
1991	1	2	3000	0.01
1992	1	2	2000	0.01
1993	1	2	2000	0.01
1994	1	2	2000	0.01
1995	1	2	2000	0.01
1996	1	2	2000	0.01
1997	1	2	1500	0.01
1998	1	2	1500	0.01
1999	1	2	1500	0.01
2000	1	2	1500	0.01
2001	1	2	1500	0.01
-9999	0	0	0	0

#
 #_CPUE_and_surveyabundance_observations
#_Units:  0=numbers; 1=biomass; 2=F; 30=spawnbio; 31=recdev; 32=spawnbio*recdev; 33=recruitment; 34=depletion(&see Qsetup); 35=parm_dev(&see Qsetup)
#_Errtype:  -1=normal; 0=lognormal; >0=T
#_SD_Report: 0=no sdreport; 1=enable sdreport
#_Fleet Units Errtype SD_Report
1 1 0 0 # FISHERY1
2 1 0 0 # FISHERY2
3 1 0 1 # SURVEY1
4 0 0 0 # SURVEY2
#_yr month fleet obs stderr
1977 7 4 339689 0.3 #_ SURVEY1
1980 7 4 193353 0.3 #_ SURVEY1
1983 7 4 151984 0.3 #_ SURVEY1
1986 7 4 55221.8 0.3 #_ SURVEY1
1989 7 4 59232.3 0.3 #_ SURVEY1
1992 7 4 31137.5 0.3 #_ SURVEY1
1995 7 4 35845.4 0.3 #_ SURVEY1
1998 7 4 27492.6 0.3 #_ SURVEY1
2001 7 4 37338.3 0.3 #_ SURVEY1
1990 7 3 5.19333 0.7 #_ SURVEY2
1991 7 3 1.1784 0.7 #_ SURVEY2
1992 7 3 5.94383 0.7 #_ SURVEY2
1993 7 3 0.770106 0.7 #_ SURVEY2
1994 7 3 16.318 0.7 #_ SURVEY2
1995 7 3 1.36339 0.7 #_ SURVEY2
1996 7 3 4.76482 0.7 #_ SURVEY2
1997 7 3 51.0707 0.7 #_ SURVEY2
1998 7 3 1.36095 0.7 #_ SURVEY2
1999 7 3 0.862531 0.7 #_ SURVEY2
2000 7 3 5.97125 0.7 #_ SURVEY2
2001 7 3 1.69379 0.7 #_ SURVEY2
-9999 1 1 1 1 # terminator for survey observations 
#
0 #_N_fleets_with_discard
#_discard_units (1=same_as_catchunits(bio/num); 2=fraction; 3=numbers)
#_discard_errtype:  >0 for DF of T-dist(read CV below); 0 for normal with CV; -1 for normal with se; -2 for lognormal; -3 for trunc normal with CV
# note: only enter units and errtype for fleets with discard 
# note: discard data is the total for an entire season, so input of month here must be to a month in that season
#_Fleet units errtype
# -9999 0 0 0.0 0.0 # terminator for discard data 
#
0 #_use meanbodysize_data (0/1)
#_COND_0 #_DF_for_meanbodysize_T-distribution_like
# note:  type=1 for mean length; type=2 for mean body weight 
#_yr month fleet part type obs stderr
#  -9999 0 0 0 0 0 0 # terminator for mean body size data 
#
# set up population length bin structure (note - irrelevant if not using size data and using empirical wtatage
2 # length bin method: 1=use databins; 2=generate from binwidth,min,max below; 3=read vector
2 # binwidth for population size comp 
10 # minimum size in the population (lower edge of first bin and size at age 0.00) 
94 # maximum size in the population (lower edge of last bin) 
1 # use length composition data (0/1)
#_mintailcomp: upper and lower distribution for females and males separately are accumulated until exceeding this level.
#_addtocomp:  after accumulation of tails; this value added to all bins
#_combM+F: males and females treated as combined gender below this bin number 
#_compressbins: accumulate upper tail by this number of bins; acts simultaneous with mintailcomp; set=0 for no forced accumulation
#_Comp_Error:  0=multinomial, 1=dirichlet
#_ParmSelect:  parm number for dirichlet
#_minsamplesize: minimum sample size; set to 1 to match 3.24, minimum value is 0.001
#
#_mintailcomp addtocomp combM+F CompressBins CompError ParmSelect minsamplesize
0 1e-07 0 0 0 0 1 #_fleet:1_FISHERY
0 1e-07 0 0 0 0 1 #_fleet:2_FISHERY
0 1e-07 0 0 0 0 1 #_fleet:3_SURVEY1
0 1e-07 0 0 0 0 1 #_fleet:4_SURVEY2
# sex codes:  0=combined; 1=use female only; 2=use male only; 3=use both as joint sexxlength distribution
# partition codes:  (0=combined; 1=discard; 2=retained
25 #_N_LengthBins; then enter lower edge of each length bin
 26 28 30 32 34 36 38 40 42 44 46 48 50 52 54 56 58 60 62 64 68 72 76 80 90
#_yr month fleet sex part Nsamp datavector(female-male)
 1971 7 1 3 0 125 0 0 0 0 0 0 0 0 0 4 1 1 2 4 1 5 6 2 3 11 8 4 5 0 0 0 0 0 0 0 0 0 0 1 0 1 3 0 3 4 2 4 5 9 17 8 3 8 0 0
 1972 7 1 3 0 125 0 0 0 0 0 0 0 0 0 3 0 1 2 1 1 6 2 7 4 10 10 4 5 3 0 0 0 0 0 0 0 0 0 1 3 2 4 1 3 1 4 4 7 3 8 11 4 10 0 0
 1973 7 1 3 0 125 0 0 0 0 0 0 0 0 0 0 0 0 7 3 4 5 6 3 10 12 6 10 9 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 1 3 0 7 2 6 7 8 5 5 3 0
 1974 7 1 3 0 125 0 0 0 0 0 0 0 0 0 2 2 0 1 1 1 4 5 3 8 8 10 4 7 0 0 0 0 0 0 0 0 0 1 2 0 4 0 0 1 5 6 6 4 6 15 11 5 0 3 0
 1975 7 1 3 0 125 0 0 0 0 0 0 0 2 1 2 1 1 3 0 2 5 6 2 3 5 9 10 10 0 0 0 0 0 0 0 0 0 0 0 4 2 2 1 2 3 5 1 4 5 13 11 6 4 0 0
 1976 7 1 3 0 125 0 0 0 0 0 0 0 2 1 0 2 2 0 3 2 3 3 3 7 18 14 4 2 2 0 0 0 0 0 0 0 1 0 0 0 0 0 1 2 4 6 6 5 7 12 6 4 3 0 0
 1977 7 1 3 0 125 0 0 0 0 0 0 0 1 0 2 0 2 2 4 0 2 6 7 5 11 7 8 5 4 0 0 0 0 0 0 2 1 3 0 1 3 3 2 0 1 4 5 3 7 7 9 5 3 0 0
 1978 7 1 3 0 125 0 0 0 0 0 0 5 1 1 1 0 1 3 1 8 4 4 6 5 9 8 3 6 5 0 0 0 0 0 0 0 0 2 1 1 2 1 1 2 2 4 1 4 1 13 9 6 4 0 0
 1979 7 1 3 0 125 0 0 0 0 0 0 0 0 0 0 3 5 2 1 5 0 5 5 2 7 4 7 5 5 0 0 0 0 0 0 0 0 0 2 2 1 3 2 7 2 4 4 5 8 10 8 6 4 1 0
 1980 7 1 3 0 125 0 0 0 0 0 0 0 4 0 0 1 0 2 4 3 2 3 2 3 16 11 12 4 2 0 0 0 0 0 0 0 0 0 1 4 1 1 2 3 5 2 6 3 1 10 11 4 2 0 0
 1981 7 1 3 0 125 0 0 0 0 0 0 1 0 0 0 3 1 2 2 4 5 2 7 3 13 9 8 4 0 0 0 0 0 0 0 2 1 1 1 2 2 3 3 1 6 1 2 1 7 5 10 6 7 0 0
 1982 7 1 3 0 125 0 0 0 0 0 0 0 0 5 2 1 3 2 3 8 2 5 4 4 6 10 11 0 0 0 0 0 0 0 0 0 0 1 0 3 0 2 1 5 6 1 8 5 5 10 5 2 5 0 0
 1983 7 1 3 0 125 0 0 0 0 0 0 0 0 0 0 7 1 1 5 4 2 2 6 2 8 13 8 6 0 0 0 0 0 0 0 0 0 0 0 4 1 0 3 3 0 4 9 5 4 7 8 6 6 0 0
 1984 7 1 3 0 125 0 0 0 0 0 0 1 0 0 4 3 0 3 1 2 5 2 4 7 11 9 6 8 0 0 0 0 0 0 0 0 0 3 3 1 1 3 3 3 2 2 4 4 8 11 4 5 2 0 0
 1985 7 1 3 0 125 0 0 0 0 0 0 0 0 1 1 2 2 5 0 3 3 5 11 4 8 9 3 2 4 0 0 0 0 0 0 0 0 1 0 1 2 0 3 8 3 4 3 8 4 13 7 4 1 0 0
 1986 7 1 3 0 125 0 0 0 3 1 0 1 2 0 4 2 0 0 4 2 8 3 5 11 5 6 6 1 0 0 0 0 0 0 0 2 2 0 1 2 1 3 4 2 3 4 6 5 5 6 4 6 5 0 0
 1987 7 1 3 0 125 0 0 0 0 1 1 1 1 1 0 2 1 6 4 2 7 6 3 5 11 9 5 4 0 0 0 0 0 0 0 0 0 2 1 0 5 2 4 3 4 4 4 2 4 7 6 5 2 0 0
 1988 7 1 3 0 125 0 0 0 0 0 2 0 1 4 2 1 1 2 2 1 7 4 5 6 9 9 2 1 0 0 0 0 0 0 0 2 1 1 3 1 3 6 3 3 0 4 5 3 5 9 9 8 0 0 0
 1989 7 1 3 0 125 0 0 0 0 0 1 0 2 1 3 3 2 1 4 4 3 4 2 3 9 5 11 2 0 0 0 0 0 0 0 0 3 6 2 1 3 0 4 3 3 2 5 7 7 9 3 3 4 0 0
 1990 7 1 3 0 125 0 0 0 0 0 0 0 2 2 2 2 2 2 2 9 4 4 6 6 8 4 4 1 0 0 0 0 0 0 0 1 1 2 2 3 8 2 8 6 6 3 2 3 4 6 5 1 2 0 0
 1991 7 1 3 0 125 0 0 0 0 0 0 0 3 0 3 3 5 5 4 3 3 0 1 6 10 4 4 0 0 0 0 0 0 0 1 1 1 1 3 4 6 5 3 5 6 6 6 6 4 7 3 3 0 0 0
 1992 7 1 3 0 125 0 0 0 0 2 2 0 1 1 1 3 3 2 7 6 4 4 2 5 6 3 6 0 0 0 0 0 0 0 0 0 0 5 3 1 3 5 3 5 8 3 4 6 3 13 4 1 0 0 0
 1993 7 1 3 0 125 0 0 0 0 0 0 1 2 2 2 2 2 4 5 10 5 7 3 2 12 7 6 0 0 0 0 0 0 0 0 0 0 3 1 1 3 2 6 4 8 4 6 4 2 4 3 1 1 0 0
 1994 7 1 3 0 125 0 0 0 0 0 0 0 0 0 4 1 4 3 4 4 9 4 6 7 8 5 3 2 0 0 0 0 0 0 0 0 0 2 0 2 1 1 4 4 10 5 8 6 3 5 6 1 3 0 0
 1995 7 1 3 0 125 0 0 0 1 0 0 1 1 1 1 2 2 5 8 4 11 5 5 4 8 7 0 0 0 0 0 0 0 0 1 0 0 1 1 3 3 1 2 6 3 4 4 8 3 12 4 3 0 0 0
 1996 7 1 3 0 125 0 0 0 1 0 2 1 0 2 4 3 3 2 3 6 6 3 3 4 11 6 6 0 0 0 0 0 0 0 0 0 1 2 0 3 3 1 0 5 4 6 7 4 5 10 3 4 1 0 0
 1997 7 1 3 0 125 0 0 0 2 0 0 2 2 0 0 3 1 6 4 6 2 9 4 5 9 12 0 0 0 0 0 0 0 0 0 3 1 0 5 3 2 4 1 1 6 4 1 6 6 5 6 4 0 0 0
 1998 7 1 3 0 125 0 0 0 0 3 1 2 2 2 2 3 1 3 6 2 0 7 4 5 12 3 1 2 0 0 0 0 0 0 4 1 1 0 2 2 0 1 1 4 6 2 5 4 6 13 7 4 1 0 0
 1999 7 1 3 0 125 0 0 0 0 1 0 1 1 3 0 1 2 2 8 3 4 7 3 5 6 5 7 0 0 0 0 0 0 0 0 0 7 3 4 2 3 2 5 2 11 3 5 1 5 7 4 2 0 0 0
 2000 7 1 3 0 125 0 0 0 0 0 1 0 0 1 2 4 3 1 6 4 4 3 3 4 5 11 0 0 0 0 0 0 0 0 0 2 4 4 3 3 6 3 4 1 8 3 5 1 4 11 1 5 5 0 0
 2001 7 1 3 0 125 0 0 0 0 2 1 0 1 1 0 2 7 6 9 4 2 5 6 4 7 6 4 0 0 0 0 0 0 0 0 2 0 1 0 2 3 2 5 3 8 3 3 5 2 10 6 3 0 0 0
 1977 7 2 3 0 125 0 0 0 0 3 0 0 2 2 3 1 2 5 0 5 6 5 3 3 8 4 10 0 0 0 0 0 0 0 0 0 6 3 3 2 2 5 2 3 3 8 1 1 6 5 8 3 2 0 0
 1980 7 2 3 0 125 0 0 0 0 1 1 1 3 2 2 1 3 6 1 2 5 1 3 3 8 3 3 4 1 0 0 0 0 0 1 1 2 3 4 4 4 4 4 1 1 1 5 3 5 14 7 5 2 0 0
 1983 7 2 3 0 125 0 0 0 0 2 3 3 5 2 4 5 2 3 2 5 5 6 5 3 3 1 8 0 0 0 0 0 0 0 2 2 1 2 2 4 2 6 2 3 5 2 4 4 1 6 10 0 0 0 0
 1986 7 2 3 0 125 0 0 0 0 2 1 1 4 6 2 3 1 1 1 5 5 5 3 3 7 7 3 2 0 0 0 0 0 1 2 1 3 2 1 5 0 2 5 6 7 3 5 2 3 7 4 4 0 0 0
 1989 7 2 3 0 125 0 0 0 0 0 5 8 3 3 5 1 2 4 1 2 2 4 3 2 3 3 2 0 0 0 0 0 0 2 2 3 5 2 5 8 8 7 3 2 4 3 6 3 1 8 0 0 0 0 0
 1992 7 2 3 0 125 0 0 0 0 0 5 6 6 5 3 2 5 6 6 5 5 1 3 1 3 4 0 0 0 0 0 0 0 0 0 2 4 3 6 5 3 6 6 2 5 4 3 1 3 1 2 3 0 0 0
 1995 7 2 3 0 125 0 0 0 0 2 0 0 4 7 5 5 5 6 2 5 6 5 6 0 3 4 1 0 0 0 0 0 0 0 2 3 0 1 2 1 5 3 4 9 5 3 3 4 2 5 4 3 0 0 0
 1998 7 2 3 0 125 0 0 0 3 1 1 2 3 4 6 4 6 5 3 1 2 1 1 1 5 2 2 0 0 0 0 0 0 0 10 5 4 2 3 7 2 1 4 4 5 3 2 3 1 8 6 2 0 0 0
 2001 7 2 3 0 125 0 0 0 0 0 2 3 5 7 5 9 2 9 5 4 4 1 1 2 2 8 0 0 0 0 0 0 0 0 2 1 4 6 5 6 4 3 4 4 5 1 3 2 1 3 2 0 0 0 0
-9999 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
#
17 #_N_age_bins
 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 20 25
2 #_N_ageerror_definitions
 0.5 1.5 2.5 3.5 4.5 5.5 6.5 7.5 8.5 9.5 10.5 11.5 12.5 13.5 14.5 15.5 16.5 17.5 18.5 19.5 20.5 21.5 22.5 23.5 24.5 25.5 26.5 27.5 28.5 29.5 30.5 31.5 32.5 33.5 34.5 35.5 36.5 37.5 38.5 39.5 40.5
 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001
 0.5 1.5 2.5 3.5 4.5 5.5 6.5 7.5 8.5 9.5 10.5 11.5 12.5 13.5 14.5 15.5 16.5 17.5 18.5 19.5 20.5 21.5 22.5 23.5 24.5 25.5 26.5 27.5 28.5 29.5 30.5 31.5 32.5 33.5 34.5 35.5 36.5 37.5 38.5 39.5 40.5
 0.5 0.65 0.67 0.7 0.73 0.76 0.8 0.84 0.88 0.92 0.97 1.03 1.09 1.16 1.23 1.32 1.41 1.51 1.62 1.75 1.89 2.05 2.23 2.45 2.71 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3
#_mintailcomp: upper and lower distribution for females and males separately are accumulated until exceeding this level.
#_addtocomp:  after accumulation of tails; this value added to all bins
#_combM+F: males and females treated as combined gender below this bin number 
#_compressbins: accumulate upper tail by this number of bins; acts simultaneous with mintailcomp; set=0 for no forced accumulation
#_Comp_Error:  0=multinomial, 1=dirichlet
#_ParmSelect:  parm number for dirichlet
#_minsamplesize: minimum sample size; set to 1 to match 3.24, minimum value is 0.001
#
#_mintailcomp addtocomp combM+F CompressBins CompError ParmSelect minsamplesize
0 1e-07 1 0 0 0 1 #_fleet:1_FISHERY
0 1e-07 1 0 0 0 1 #_fleet:2_FISHERY
0 1e-07 1 0 0 0 1 #_fleet:3_SURVEY1
0 1e-07 1 0 0 0 1 #_fleet:4_SURVEY2
1 #_Lbin_method_for_Age_Data: 1=poplenbins; 2=datalenbins; 3=lengths
# sex codes:  0=combined; 1=use female only; 2=use male only; 3=use both as joint sexxlength distribution
# partition codes:  (0=combined; 1=discard; 2=retained
#_yr month fleet sex part ageerr Lbin_lo Lbin_hi Nsamp datavector(female-male)
 1971 7 1 3 0 2 1 -1 75 0 0 0 0 3 1 1 4 2 1 0 1 2 2 13 2 3 0 0 4 2 1 1 2 1 2 2 1 2 1 2 6 5 8
 1972 7 1 3 0 2 1 -1 75 2 1 1 1 0 3 1 2 2 5 3 1 2 2 9 8 3 0 0 1 2 3 1 3 0 5 1 3 0 2 1 2 3 2
 1973 7 1 3 0 2 1 -1 75 0 0 1 0 1 1 2 3 3 1 1 5 2 2 7 4 3 0 0 0 4 1 3 5 1 2 3 1 3 2 0 5 3 6
 1974 7 1 3 0 2 1 -1 75 0 0 2 0 1 4 2 2 2 4 1 1 1 2 6 6 6 0 0 4 1 2 2 1 2 0 0 1 2 1 1 6 5 7
 1975 7 1 3 0 2 1 -1 75 0 0 1 2 3 1 1 1 2 1 2 2 2 3 10 3 4 0 0 0 0 10 1 2 3 2 1 0 0 0 0 9 3 6
 1976 7 1 3 0 2 1 -1 75 0 0 1 0 2 2 2 1 3 1 2 3 1 1 7 1 3 0 0 0 0 7 4 3 2 1 2 4 4 0 0 8 10 0
 1977 7 1 3 0 2 1 -1 75 0 0 0 0 7 1 0 0 2 4 2 2 3 1 7 2 3 0 0 2 1 4 2 3 3 4 2 2 2 0 1 8 3 4
 1978 7 1 3 0 2 1 -1 75 0 0 3 2 1 1 0 2 0 2 4 3 1 0 9 4 6 0 0 2 2 5 1 0 2 3 2 4 2 0 4 4 3 3
 1979 7 1 3 0 2 1 -1 75 2 0 1 5 2 1 2 3 3 3 2 2 1 0 3 7 0 0 0 2 0 1 0 2 3 2 5 1 3 1 2 6 9 1
 1980 7 1 3 0 2 1 -1 75 0 1 0 2 0 1 1 2 2 3 2 1 1 0 7 8 0 0 0 0 3 2 1 1 1 2 2 4 2 2 2 11 3 8
 1981 7 1 3 0 2 1 -1 75 0 4 0 3 7 2 2 2 2 1 1 2 2 1 4 4 6 0 0 3 2 2 1 1 3 2 2 0 1 2 2 5 3 3
 1982 7 1 3 0 2 1 -1 75 0 2 1 1 3 3 2 1 1 2 2 1 0 2 6 3 9 0 0 0 0 3 5 0 1 4 1 1 1 2 1 8 9 0
 1983 7 1 3 0 2 1 -1 75 0 0 0 6 1 2 2 2 1 1 4 5 0 0 6 2 7 0 0 3 1 3 5 1 0 1 1 3 0 3 3 5 3 4
 1984 7 1 3 0 2 1 -1 75 0 0 0 3 4 0 3 6 3 1 4 0 2 0 7 2 3 0 0 3 1 5 4 2 3 5 1 2 1 2 0 1 2 5
 1985 7 1 3 0 2 1 -1 75 0 0 0 5 1 2 4 5 0 2 4 3 2 3 3 4 5 0 0 0 1 2 3 2 4 2 0 2 3 1 1 7 2 2
 1986 7 1 3 0 2 1 -1 75 0 2 2 1 3 7 4 3 2 2 2 2 2 0 4 2 2 0 0 0 0 4 4 4 1 2 3 4 0 0 1 5 7 0
 1987 7 1 3 0 2 1 -1 75 0 3 1 3 1 2 3 4 2 3 3 2 2 1 3 2 0 0 0 7 1 5 1 4 2 4 3 2 3 1 0 2 1 4
 1988 7 1 3 0 2 1 -1 75 1 0 5 0 2 3 3 3 4 3 3 1 0 3 3 5 0 0 1 3 3 2 2 1 4 3 2 1 2 4 0 5 3 0
 1989 7 1 3 0 2 1 -1 75 0 3 1 1 4 3 7 1 5 1 1 4 1 0 1 7 0 0 0 5 3 4 1 1 5 3 1 5 2 1 0 2 2 0
 1990 7 1 3 0 2 1 -1 75 0 0 7 3 7 3 0 1 3 0 1 1 1 1 3 4 0 0 1 0 8 4 3 3 2 4 5 1 5 1 0 1 2 0
 1991 7 1 3 0 2 1 -1 75 0 0 4 1 7 4 2 3 2 1 0 1 1 3 3 3 0 0 3 4 2 5 4 4 1 3 3 0 4 2 0 4 1 0
 1992 7 1 3 0 2 1 -1 75 0 0 7 4 5 10 4 3 0 3 1 0 2 0 2 1 1 0 0 5 1 3 8 3 3 1 2 0 1 3 0 1 1 0
 1993 7 1 3 0 2 1 -1 75 0 0 7 4 3 7 5 7 2 1 0 1 0 4 0 0 0 0 0 3 3 4 3 7 0 0 4 2 1 1 1 5 0 0
 1994 7 1 3 0 2 1 -1 75 0 0 3 6 4 4 4 9 4 5 1 0 0 0 0 0 3 0 0 0 9 0 7 2 2 3 4 0 3 2 0 0 0 0
 1995 7 1 3 0 2 1 -1 75 3 1 2 0 8 5 2 6 2 5 0 2 1 4 0 0 0 0 0 0 2 5 3 2 3 5 6 1 0 1 1 3 1 1
 1996 7 1 3 0 2 1 -1 75 0 0 1 1 5 4 3 7 2 3 2 3 3 1 5 1 0 0 2 5 0 5 4 1 2 3 4 2 3 0 1 2 0 0
 1997 7 1 3 0 2 1 -1 75 0 5 3 5 0 2 4 3 4 5 1 1 3 2 2 0 0 0 0 0 3 1 6 5 5 2 3 4 1 2 3 0 0 0
 1998 7 1 3 0 2 1 -1 75 5 3 1 4 1 2 3 4 3 2 0 2 0 1 5 0 0 0 0 4 6 4 2 7 2 1 1 6 3 0 0 2 1 0
 1999 7 1 3 0 2 1 -1 75 2 2 3 3 6 3 3 3 8 3 3 3 0 1 1 0 0 0 1 3 3 3 5 4 0 4 2 4 0 1 0 1 0 0
 2000 7 1 3 0 2 1 -1 75 0 2 1 9 4 4 2 2 4 3 1 0 1 0 5 0 0 0 0 8 11 3 1 2 2 1 1 2 1 0 2 3 0 0
 2001 7 1 3 0 2 1 -1 75 0 1 1 6 8 1 1 0 5 2 2 2 0 3 4 0 0 0 0 5 3 4 6 3 3 1 4 3 1 1 2 3 0 0
 1977 7 2 3 0 2 1 -1 75 2 1 2 1 0 4 3 3 2 1 1 0 1 1 4 7 0 0 2 2 7 1 0 1 0 1 2 4 1 2 2 7 10 0
 1980 7 2 3 0 2 1 -1 75 3 3 4 6 5 2 0 2 3 0 3 2 2 2 2 1 4 0 2 3 5 3 1 2 1 1 2 1 1 1 0 3 1 4
 1983 7 2 3 0 2 1 -1 75 3 4 3 2 3 0 0 7 0 0 3 1 1 0 5 6 0 0 2 2 4 1 2 3 4 3 2 0 1 1 2 7 1 2
 1986 7 2 3 0 2 1 -1 75 3 0 2 5 3 5 5 3 1 3 2 1 1 1 3 0 2 0 0 2 3 6 6 1 3 3 1 1 1 1 2 2 3 0
 1989 7 2 3 0 2 1 -1 75 7 3 7 3 2 1 0 3 2 1 2 1 1 5 0 0 0 0 4 8 6 1 2 3 5 1 1 2 0 4 0 0 0 0
 1992 7 2 3 0 2 1 -1 75 2 5 3 4 0 5 0 5 2 0 0 0 1 0 3 0 0 0 4 5 5 10 8 6 2 1 2 0 0 1 0 1 0 0
 1995 7 2 3 0 2 1 -1 75 0 5 2 3 2 3 5 4 2 1 1 2 0 0 3 0 0 0 2 3 5 11 2 6 5 1 2 1 2 0 0 2 0 0
 1998 7 2 3 0 2 1 -1 75 9 4 4 3 1 1 1 1 3 3 1 2 1 7 0 0 0 0 6 5 3 5 1 3 3 2 3 2 0 1 0 0 0 0
 2001 7 2 3 0 2 1 -1 75 4 0 4 11 5 3 4 2 2 0 0 0 0 0 2 0 0 0 2 4 7 11 5 2 0 2 2 2 0 0 0 1 0 0
-9999  0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
#
1 #_Use_MeanSize-at-Age_obs (0/1)
# sex codes:  0=combined; 1=use female only; 2=use male only; 3=use both as joint sexxlength distribution
# partition codes:  (0=combined; 1=discard; 2=retained
# ageerr codes:  positive means mean length-at-age; negative means mean bodywt_at_age
#_yr month fleet sex part ageerr ignore datavector(female-male)
#                                          samplesize(female-male)
 1971 7 1 3 0 1 2 29.8931 40.6872 44.7411 50.027 52.5794 56.1489 57.1033 61.1728 61.7417 63.368 64.4088 65.6889 67.616 68.5972 69.9177 71.0443 72.3609 32.8188 39.5964 43.988 50.1693 53.1729 54.9822 55.3463 60.3509 60.7439 62.3432 64.3224 65.1032 64.1965 66.7452 67.5154 70.8749 71.2768 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20
 1995 7 1 3 0 1 2 32.8974 38.2709 43.8878 49.2745 53.5343 55.1978 57.4389 62.0368 62.1445 62.9579 65.0857 65.6433 66.082 65.6117 67.0784 69.3493 72.2966 32.6552 40.5546 44.6292 50.4063 52.0796 56.1529 56.9004 60.218 61.5894 63.6613 64.0222 63.4926 65.8115 69.5357 68.2448 66.881 71.5122 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20
 1971 7 2 3 0 1 2 34.1574 38.8017 43.122 47.2042 49.0502 51.6446 56.3201 56.3038 60.5509 60.2537 59.8042 62.9309 66.842 67.8089 71.1612 70.7693 74.5593 35.3811 40.7375 44.5192 47.6261 52.5298 53.5552 54.9851 58.9231 58.9932 61.8625 64.0366 62.7507 63.9754 64.5102 66.9779 67.7361 69.1298 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20
 1995 7 2 3 0 1 2 34.6022 38.3176 42.9052 48.2752 50.6189 53.476 56.7806 59.4127 60.5964 60.5537 65.3608 64.7263 67.4315 67.1405 68.9908 71.9886 74.1594 35.169 40.2404 43.8878 47.3519 49.9906 52.2207 54.9035 58.6058 60.0957 62.4046 62.2298 62.1437 66.2116 65.7657 69.9544 70.6518 71.4371 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20
-9999  0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
#
0 #_N_environ_variables
# -2 in yr will subtract mean for that env_var; -1 will subtract mean and divide by stddev (e.g. Z-score)
#Yr Variable Value
#
0 # N sizefreq methods to read 
#
0 # do tags (0/1)
#
0 #    morphcomp data(0/1) 
#  Nobs, Nmorphs, mincomp
#  yr, seas, type, partition, Nsamp, datavector_by_Nmorphs
#
0  #  Do dataread for selectivity priors(0/1)
# Yr, Seas, Fleet,  Age/Size,  Bin,  selex_prior,  prior_sd
# feature not yet implemented
#
999

ENDDATA


*/