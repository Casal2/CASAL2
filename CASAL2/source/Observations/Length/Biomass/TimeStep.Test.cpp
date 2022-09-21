/**
 * @file Biomass.Test.cpp
 * @author  C.Marsh
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "TimeStep.h"

#include <iostream>

#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Observations/Manager.h"
#include "TestResources/TestFixtures/InternalEmptyLengthModel.h"

// Namespaces
namespace niwa {
namespace length {

using niwa::testfixtures::InternalEmptyLengthModel;
using std::cout;
using std::endl;

const std::string test_cases_length_based_observation_biomass =
    R"(
@model
type length
start_year 1986 
final_year 2012
projection_final_year 2021
length_bins  1:68
length_plus t
length_plus_group 69
base_weight_units tonnes
initialisation_phases Equilibrium_state
time_steps Annual

@categories 
format tag
names untagged tag_1996
growth_increment growth_model growth_model

@initialisation_phase Equilibrium_state
type iterative
years 200
convergence_years 200

@time_step Annual 
processes Recruit_BH growth  mortality tag_1996

@process Recruit_BH
type recruitment_beverton_holt
ssb_offset 1
standardise_years 1986:2010
recruitment_multipliers 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00  1.00 1.00 1.00  1.00 1.00 1.00
b0 1500
categories untagged
proportions 1
steepness 0.75
inital_mean_length 10
inital_length_cv 0.40
ssb SSB


@process tag_1996
type tagging
years 1996
from untagged
to tag_1996
initial_mortality 0
u_max 0.9
selectivities [type=constant; c=1]
penalty none
N 40
table proportions
year 1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25	26	27	28	29	30	31	32	33	34	35	36	37	38	39	40	41	42	43	44	45	46	47	48	49	50	51	52	53	54	55	56	57	58	59	60	61	62	63	64	65	66	67	68
1996 0	0	0	0	0	0	0	0		0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0.025	0	0.05	0.025	0	0	0.025	0.05	0	0.05	0.175	0.15	0.025	0.025	0.075	0.05	0.025	0.075	0.05	0.025	0.025	0.05	0	0.025	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0 0 
end_table

@process mortality
type mortality_instantaneous
m 0.2 * 2
relative_m_by_length One * 2
time_step_proportions 1
categories *
table catches
year Trawl_1
1987	0
1988	0
1989	0
1990	16.19729814
1991	136.5431735
1992	64.99411365
1993	73.01365139
1994	29.60436938
1995	66.48247047
1996	107.1336092
1997	60.94452902
1998	150.8267401
1999	126.7738246
2000	101.141994
2001	28.29833853
2002	65.25326787
2003	77.43894769
2004	39.37986312
2005	33.22696512
2006	35.71771887
2007	35.93582888
2008	55.00361249
2009	31.55442523
2010	85.05248259
2011	81.7369449
2012	60.51004034
end_table
table method
method        category  selectivity 	 u_max  time_step  penalty
Trawl_1  untagged  		FishingSel_1     0.9   	Annual    	none
Trawl_1  tag_1996  		FishingSel_1     0.9   	Annual    	none
end_table


@process growth
type growth
categories untagged tag_1996

@growth_increment growth_model
type basic
l_alpha 20
l_beta  40
g_alpha 10
g_beta 1
min_sigma 2
distribution normal
length_weight allometric
cv 0.0
compatibility_option casal

@length_weight allometric
type basic
a 0.000000000373
b 3.145
units tonnes

@derived_quantity SSB
type biomass
categories untagged tag_1996
selectivities maturity * 2
time_step Annual
time_step_proportion 0.5

@selectivity double_normal
type double_normal
mu 21
sigma_l 5
sigma_r 10

@selectivity maturity
type logistic
a50 30 
ato95 5

@selectivity FishingSel_1
type logistic
a50 24 
ato95 5

@selectivity One
type constant
c 1

@observation CPUE_Trawl_1
type biomass
time_step Annual
time_step_proportion 0.5
catchability CPUE-Commercialq_1
categories untagged+tag_1996
selectivities FishingSel_1 * 2
likelihood lognormal
years 1991  1992  1993  1994  1995  1996  1997  1998  1999  2000  2001  2002  2003  2004  2005  2006  2007  2008  2009 2010 2011 2012
table obs
1991	1.3275752 0.25
1992	0.9873441 0.25
1993	1.5487283 0.25
1994	1.2561366 0.25
1995	1.9125031 0.25
1996	2.4433051 0.25
1997	1.8569732 0.25
1998	1.6487923 0.25
1999	1.3470876 0.25
2000	1.065784 0.25
2001	0.917451 0.25
2002	0.9752099 0.25
2003	0.7673474 0.25
2004	0.8045571 0.25
2005	0.8154302 0.25
2006	0.8506109 0.25
2007	0.7674857 0.25
2008	0.683983 0.25
2009	0.9247193 0.25
2010	0.9774631 0.25
2011	1.0770538 0.25
2012	1.2012309 0.25
end_table

@catchability CPUE-Commercialq_1
type free
q 0.00231
)";

/**
 *
 */
TEST_F(InternalEmptyLengthModel, Observation_Biomass_Length) {
  AddConfigurationLine(test_cases_length_based_observation_biomass, __FILE__, 32);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_NEAR(107.525, obj_function.score(), 1e-3);

  Observation* observation = model_->managers()->observation()->GetObservation("CPUE_Trawl_1");

  map<unsigned, vector<obs::Comparison> >& comparisons = observation->comparisons();
  // these values are from CASAL the config is below
  vector<double>   expected_vals = {3.44886, 3.22994, 3.11268, 3.04622, 3.00605, 2.88496, 2.77301, 2.63803, 2.42906, 2.29566, 2.28474,
                                    2.33494, 2.32469, 2.33879, 2.40748, 2.47713, 2.53735, 2.57141, 2.60089, 2.60103, 2.53537, 2.49959};
  vector<unsigned> years         = {1991, 1992, 1993, 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012};

  for (unsigned i = 0; i < expected_vals.size(); i++) {
    EXPECT_NEAR(expected_vals[i], comparisons[years[i]][0].expected_, 1e-4);
  }
}

/*
The CASAL config to reproduce the above unit-test
------------------
population.csl
------------------
@initialization
B0 1500
@n_equilibrium 200 # had plus-group growth >5% between year 99 and year 100
@weightless_model False
@size_based True
@n_classes 68
# class nos.   1    2    3    4    5    6    7    8    9  10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31  32  33  34  35  36  37  38  39  40
41  42  43  44  45  46  47  48  49  50  51  52  53  54  55  56  57  58  59  60  61  62  63  64  65 66 67 68
@class_mins    1    2    3    4    5    6    7    8    9  10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31  32  33  34  35  36  37  38  39  40
41  42  43  44  45  46  47  48  49  50  51  52  53  54  55  56  57  58  59  60  61  62  63  64  65 66 67 68
@plus_group_size 69
@plus_group T
@sex_partition F
@mature_partition False
@n_areas 1
@n_stocks 1
@n_tags 1

@tag_names tag-data-all1 #tag-data-all2 tag-data-all3
@tag tag-data-all1
tag_name tag-data-all1
year 1996
step 2
mature_only False
number 40
plus_group T
props_all 0	0	0	0	0	0	0	0		0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0.025	0	0.05	0.025	0	0	0.025	0.05	0	0.05	0.175	0.15	0.025	0.025	0.075	0.05
0.025	0.075	0.05	0.025	0.025	0.05	0	0.025	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0 0

@initial 1986
@current 2012
@final 2015

@annual_cycle
time_steps 1 # 1=Oct-Jan, 2=Feb-Apr, 3=May-Sept
recruitment_time 1
spawning_time 1
spawning_part_mort 0.5
spawning_p 1
n_growths 1
growth_times 1
M_props 1
baranov False
fishery_names Trawl_1 #Trawl_2 Trawl_3
fishery_times 1 #1 1

@y_enter 1
@standardise_YCS True #Haist parameterisation

@recruitment
n_rinitial 0
YCS_years 1985 1986 1987 1988 1989 1990 1991 1992 1993 1994 1995 1996 1997 1998 1999 2000 2001 2002 2003 2004 2005  2006 2007 2008  2009 2010 2011
YCS       1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00  1.00 1.00 1.00  1.00 1.00 1.00
SR BH
steepness           0.75
initial_size_mean   10.0  # WAS 1.0 needs checking
initial_size_cv    0.40
first_free      1985
last_free       2009
year_range      1985 2009

@randomisation_method lognormal-empirical
@first_random_year 2010

@growth #step 1 growth
type basic
l         20     40
g     10 1  #   9.76   2.08
cv         0.00
minsigma   2   #   1.17

@maturity_props
all logistic    30 5

@natural_mortality
all             0.20

@fishery Trawl_1
years     1987  1988  1989  1990  1991  1992  1993  1994  1995  1996  1997  1998  1999  2000  2001  2002  2003  2004  2005  2006  2007  2008 2009  2010 2011 2012
#catches 0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
catches 0	0	0	16.19729814	136.5431735	64.99411365	73.01365139	29.60436938	66.48247047	107.1336092	60.94452902	150.8267401	126.7738246
101.141994	28.29833853	65.25326787	77.43894769	39.37986312	33.22696512	35.71771887	35.93582888	55.00361249	31.55442523	85.05248259	81.7369449	60.51004034 future_constant_catches
58 selectivity FishingSel_1 U_max 0.9

@selectivity_names FishingSel_1 FishingSel_2  TrawlSurveySel_1  TrawlSurveySel_2   PhotoSurveySel   ssb-bio

@selectivity FishingSel_1
all logistic 24 5

@selectivity FishingSel_2
all Tuck_logistic_m 35 5  1.0 1.0


@selectivity TrawlSurveySel_1
all Tuck_logistic_m 30 3.5  1.0 1.0


@selectivity TrawlSurveySel_2
all Tuck_logistic_m 30 3.5  1.0 1.0

@selectivity PhotoSurveySel
all logistic_capped 20 3.5 1.0


@selectivity ssb-bio
 all  knife_edge  10


@size_weight # Cryer & Stotter (1997)
a   0.000000000373
b   3.145
verify_size_weight 50 0.001 0.1

------------------
estimation.csl
------------------
@estimator Bayes
@max_iters 1000 # initial 300
@max_evals 2000 # initial 1000
@grad_tol 0.0002 #The default is 0.002

@MCMC
start 1
length 3000000
keep 2000
adaptive_stepsize True
adapt_at 100000 200000 400000
burn_in 500
proposal_t True
df 4
stepsize 0.01

@relative_abundance CPUE-Trawl_1 #  revised for core areas only (updated IDT)
years     1991  1992  1993  1994  1995  1996  1997  1998  1999  2000  2001  2002  2003  2004  2005  2006  2007  2008  2009 2010 2011 2012
step 1
proportion_mortality 0.5
q CPUE-Commercialq_1
biomass True
ogive FishingSel_1

1991	1.3275752
1992	0.9873441
1993	1.5487283
1994	1.2561366
1995	1.9125031
1996	2.4433051
1997	1.8569732
1998	1.6487923
1999	1.3470876
2000	1.065784
2001	0.917451
2002	0.9752099
2003	0.7673474
2004	0.8045571
2005	0.8154302
2006	0.8506109
2007	0.7674857
2008	0.683983
2009	0.9247193
2010	0.9774631
2011	1.0770538
2012	1.2012309

cv 0.25

dist lognormal


# RELATIVITY CONSTANTS FIRST
@q_method free

@q CPUE-Commercialq_1
q 0.00231

#FREE PARAMETERS FROM HERE ON
@estimate
parameter initialization.B0
phase 1
lower_bound    500
upper_bound  20000
prior uniform

*/

}  // namespace length
} /* namespace niwa */

#endif /* TESTMODE */
