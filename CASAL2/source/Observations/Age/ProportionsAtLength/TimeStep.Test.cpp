/**
 * @file ProportionsAtLength.Test.cpp
 * @author  C Marsh
 * @date 21/08/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "TimeStep.h"

#include <iostream>

#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Observations/Manager.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"

// Namespaces
namespace niwa {
namespace age {

using niwa::testfixtures::InternalEmptyModel;
using std::cout;
using std::endl;

const std::string test_cases_observation_proportions_at_length_single =
    R"(
@model
start_year 1990
final_year 1997
min_age 1
max_age 4
age_plus true
base_weight_units kgs
initialisation_phases iphase1
time_steps init step1 step2 step3
length_bins 0 20 40 60 80 110
length_plus f

@categories
format stock
names stock
age_lengths age_size

@initialisation_phase iphase1
type iterative
years 100

@time_step init
processes [type=null_process]

@time_step step1
processes Recruitment instant_mort

@time_step step2
processes instant_mort

@time_step step3
processes  Ageing instant_mort

@process Recruitment
type recruitment_beverton_holt
categories stock
proportions 1
r0 4.04838e+006
ycs_years  1989:1996
standardise_ycs_years 1989 1990 1991 1992 1993 1994 1995 1996
ycs_values      1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00
steepness 0.9
ssb biomass_t1
age 1

@process Ageing
type ageing
categories stock

@process instant_mort
type mortality_instantaneous
m 0.19
time_step_proportions 0.42 0.25 0.33
relative_m_by_age One
categories stock
table catches
year FishingWest FishingEest
1991  309000  689000
1992  409000  503000
1993  718000  1087000
1994  656000  1996000
1995  368000  2912000
1996  597000  2903000
1997  1353000 2483000
end_table

table method
method       category  selectivity u_max   time_step penalty
FishingWest   stock     westFSel    0.7     step1     none
FishingEest   stock     eastFSel    0.7     step1     none
end_table

@derived_quantity biomass_t1
type biomass
time_step step1
categories stock
time_step_proportion 1.0
selectivities MaturationSel

@selectivity One
type constant
c 1

@selectivity MaturationSel
type all_values_bounded
l 2
h 4
v 0.50 0.75 0.97

@selectivity westFSel
type double_normal
mu 6
sigma_l 3
sigma_r 10
alpha 1.0

@selectivity eastFSel
type double_normal
mu 6
sigma_l 3
sigma_r 10
alpha 1.0

@selectivity log
type logistic
a50 6
ato95 3
alpha 1.0

@selectivity chatTANSel
type double_normal
mu 6
sigma_l 3
sigma_r 10
alpha 1.0

@age_length age_size
type von_bertalanffy
by_length True
k  0.5
t0 -0.21
Linf 88.3
cv_first 0.1
length_weight size_weight3

@length_weight size_weight3
type basic
units kgs
a 2.0e-6
b 3.288

@observation observation
type proportions_at_length
years 1990 1992 1993 1994 1995
likelihood multinomial
time_step step1
categories stock
time_step_proportion 1
selectivities chatTANSel
delta 1e-5
table obs
1990    0.2   0.3   0.1   0.2   0.2
1992    0.12  0.25    0.28    0.25    0.1
1993    0.0   0.05    0.05    0.10    0.80
1994    0.05    0.1   0.05    0.05    0.75
1995    0.3   0.4   0.2   0.05    0.05
end_table
table error_values
1990 25
1992 37
1993 31
1994 34
1995 22
end_table

@report DQ
type derived_quantity
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Observation_Proportions_At_Length_Single) {
  AddConfigurationLine(test_cases_observation_proportions_at_length_single, __FILE__, 31);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(312.92195125167586, obj_function.score());

  Observation* observation = model_->managers()->observation()->GetObservation("observation");

  map<unsigned, vector<obs::Comparison> >& comparisons = observation->comparisons();
  ASSERT_EQ(5u, comparisons.size());

  unsigned year = 1992;
  ASSERT_FALSE(comparisons.find(year) == comparisons.end());
  ASSERT_EQ(5u, comparisons[year].size());
  EXPECT_EQ("stock", comparisons[year][0].category_);
  EXPECT_DOUBLE_EQ(37, comparisons[year][0].error_value_);
  EXPECT_DOUBLE_EQ(1.3049322854316964e-08, comparisons[year][0].expected_);
  EXPECT_DOUBLE_EQ(0.12, comparisons[year][0].observed_);
  EXPECT_DOUBLE_EQ(58.053573280444361, comparisons[year][0].score_);

  EXPECT_EQ("stock", comparisons[year][1].category_);
  EXPECT_DOUBLE_EQ(37, comparisons[year][1].error_value_);
  EXPECT_DOUBLE_EQ(0.023617445143820415, comparisons[year][1].expected_);
  EXPECT_DOUBLE_EQ(0.25, comparisons[year][1].observed_);
  EXPECT_DOUBLE_EQ(48.016392832372198, comparisons[year][1].score_);

  EXPECT_EQ("stock", comparisons[year][2].category_);
  EXPECT_DOUBLE_EQ(37, comparisons[year][2].error_value_);
  EXPECT_DOUBLE_EQ(0.08510298901271289, comparisons[year][2].expected_);
  EXPECT_DOUBLE_EQ(0.28, comparisons[year][2].observed_);
  EXPECT_DOUBLE_EQ(41.483073819928485, comparisons[year][2].score_);

  EXPECT_EQ("stock", comparisons[year][3].category_);
  EXPECT_DOUBLE_EQ(37, comparisons[year][3].error_value_);
  EXPECT_DOUBLE_EQ(0.59473940098840894, comparisons[year][3].expected_);
  EXPECT_DOUBLE_EQ(0.25, comparisons[year][3].observed_);
  EXPECT_DOUBLE_EQ(18.174619217398927, comparisons[year][3].score_);

  EXPECT_EQ("stock", comparisons[year][4].category_);
  EXPECT_DOUBLE_EQ(37, comparisons[year][4].error_value_);
  EXPECT_DOUBLE_EQ(0.29654015180573506, comparisons[year][4].expected_);
  EXPECT_DOUBLE_EQ(0.1, comparisons[year][4].observed_);
  EXPECT_DOUBLE_EQ(7.2340239530511967, comparisons[year][4].score_);
}

const std::string hak_like_model =
R"(
@model
min_age 1
max_age 30
age_plus true
base_weight_units tonnes
initialisation_phases Equilibrium_phase
time_steps Jul_Jan Feb_Jun
length_bins 10 20 30 40 50 60 70 80 90 100 110 120
length_plus true
start_year 1975
final_year 2018
projection_final_year 2022


@categories
format sex
names male female
age_lengths age_len_m age_len_f


@initialisation_phase Equilibrium_phase
type Derived
casal_initialisation_switch true

@time_step Jul_Jan
processes Ageing Recruitment Instantaneous_Mortality

@time_step Feb_Jun
processes Instantaneous_Mortality



@process Recruitment
type recruitment_beverton_holt
categories male female
proportions 0.5 0.5
b0 60000
standardise_ycs_years          1974:2013
ycs_values 1.00 1.00 1.00 1.00 1.92 1.11 0.78 0.71 1.00 0.38 0.89 0.66 1.08 0.84 1.06 1.06 1.19 1.31 1.71 0.93 1.92 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00
ycs_years  1974:2017
steepness 0.8
ssb SSB
age 1


@process Ageing
type ageing
categories male female


@process Instantaneous_Mortality
type mortality_instantaneous
m 0.19
time_step_proportions 0.58 0.42
relative_m_by_age One
categories *
table catches
year    subaF
1975    120
1976    281
1977    372
1978    762
1979    364
1980    350
1981    272
1982    179
1983    448
1984    722
1985    525
1986    818
1987    713
1988    1095
1989    1237
1990    1897
1991    2381
1992    2810
1993    3941
1994    1596
1995    1995
1996    2779
1997    1915
1998    2958
1999    2853
2000    3109
2001    2820
2002    2445
2003    2776
2004    3221
2005    2580
2006    2566
2007    1709
2008    2329
2009    2446
2010    1926
2011    1319
2012    1900
2013    1859
2014    1839
2015    1600
2016    1464
2017    1033
2018    1033
end_table

table method
method    category  selectivity  u_max  time_step  penalty
subaF     *         subaFsel     0.7    Jul_Jan    fisheryCatchMustBeTaken
end_table

@derived_quantity SSB
type biomass
time_step Jul_Jan
categories male female
time_step_proportion 0.5
time_step_proportion_method weighted_sum
selectivities maturity_sel_m maturity_sel_f



@selectivity maturity_sel_m
type all_values_bounded
l 2
h 15
v 0.01 0.03    0.09    0.22    0.46    0.71    0.88    0.96    0.98    0.99    1.00    1.00    1.00    1.00


@selectivity maturity_sel_f
type all_values_bounded
l 2
h 15
v 0.01 0.02    0.05    0.11    0.23    0.43    0.64    0.81    0.91    0.96    0.98    0.99    1.00    1.00



@selectivity subaFsel
type logistic
a50 4
ato95 3
alpha 1


@selectivity subaTANselA
type logistic
a50 10
ato95 5
alpha 1


@selectivity One
type constant
c 1.0


@age_length age_len_m
type schnute
compatibility_option casal
by_length true
time_step_proportions 0.0 0.33
y1 22.3
y2 89.8
tau1 1
tau2 20
a 0.249
b 1.243
cv_first 0.1
distribution normal
length_weight len_wt_m

@age_length age_len_f
type schnute
compatibility_option casal
by_length true
time_step_proportions 0.0 0.33
y1 22.9
y2 109.9
tau1 1
tau2 20
a 0.147
b 1.457
cv_first 0.1
distribution normal
length_weight len_wt_f


@length_weight len_wt_m
type basic
units tonnes
a 2.13e-9
b 3.281

@length_weight len_wt_f
type basic
units tonnes
a 1.83e-9
b 3.314


@penalty fisheryCatchMustBeTaken
type process
log_scale True
multiplier 1000
)";


const std::string prop_at_length_plus_group =
R"(
@observation subaTANageDEC_len
type proportions_at_length
time_step Jul_Jan
time_step_proportion 0.4
categories male+female
selectivities subaTANselA
likelihood multinomial
delta 1e-11
length_bins 10	30	50	60	70	80	90	100	110	120
plus_group true
years 1990 
table obs
1990	0.002458274	0.027817311	0.035580282	0.020442489	0.073489455	0.064562039	0.129512227	0.340147496	0.172208565	0.133781861
end_table
table error_values
1990 19
end_table
)";


const std::string prop_at_length_expect_fail_incorrect_bespoke_length_bins =
R"(
@observation subaTANageDEC_len
type proportions_at_length
time_step Jul_Jan
time_step_proportion 0.4
categories male+female
selectivities subaTANselA
likelihood multinomial
delta 1e-11
length_bins 10	31	50	60	70	80	90	100	110	120
plus_group true
years 1990 
table obs
1990	0.002458274	0.027817311	0.035580282	0.020442489	0.073489455	0.064562039	0.129512227	0.340147496	0.172208565	0.133781861
end_table
table error_values
1990 19
end_table
)";

const std::string prop_at_length_expect_fail_incorrect_proportions_supplied =
R"(
@observation subaTANageDEC_len
type proportions_at_length
time_step Jul_Jan
time_step_proportion 0.4
categories male+female
selectivities subaTANselA
likelihood multinomial
delta 1e-11
length_bins 10	31	50	60	70	80	90	100	110	120
plus_group true
years 1990 
table obs
1990	0.002458274	0.027817311	0.035580282	0.020442489	0.073489455	0.064562039	0.129512227	0.340147496	0.172208565
end_table
table error_values
1990 19
end_table
)";

const std::string prop_at_length_no_plus_group =
R"(
@observation subaTANageDEC_len
type proportions_at_length
time_step Jul_Jan
time_step_proportion 0.4
categories male+female
selectivities subaTANselA
likelihood multinomial
delta 1e-11
length_bins 10	30	50	60	70	80	90	100	110	120
plus_group false
years 1990 
table obs
1990	0.002837939	0.032113518	0.041075429	0.023599701	0.084839432	0.074533234	0.149514563	0.392681105	0.198805078
end_table
table error_values
1990 19
end_table
)";


/**
 * Test bespoke length bin functionality in this class
 */
TEST_F(InternalEmptyModel, Observation_Proportions_At_Length_bespoke_length_bins_w_plus_group) {
  AddConfigurationLine(hak_like_model, __FILE__, 31);
  AddConfigurationLine(prop_at_length_plus_group, __FILE__, 31);
  LoadConfiguration();
  model_->Start(RunMode::kBasic);
  Observation* observation = model_->managers()->observation()->GetObservation("subaTANageDEC_len");
  map<unsigned, vector<obs::Comparison> >& comparisons = observation->comparisons();
  ASSERT_EQ(1u, comparisons.size()); // only one year of data
  // from CASAL model with same configuration
  vector<Double> expected_vals = {0.0050132,	0.0107304,	0.0140647,	0.0388503,	0.129307,	0.259102,	0.258556,	0.167642,	0.0838241,	0.0329095};
  unsigned year = 1990;
  ASSERT_FALSE(comparisons.find(year) == comparisons.end());
  ASSERT_EQ(10, comparisons[year].size()); // 10 length bins
  EXPECT_EQ("male+female", comparisons[year][0].category_);
  for(unsigned i = 0; i < expected_vals.size(); ++i) {
    EXPECT_NEAR(expected_vals[i], comparisons[year][i].expected_, 0.0001);
  }
}
/**
 * Test bespoke length bin functionality in this class
 */
TEST_F(InternalEmptyModel, Observation_Proportions_At_Length_bespoke_length_bins_no_plus_group) {
  AddConfigurationLine(hak_like_model, __FILE__, 31);
  AddConfigurationLine(prop_at_length_no_plus_group, __FILE__, 31);
  LoadConfiguration();
  model_->Start(RunMode::kBasic);
  Observation* observation = model_->managers()->observation()->GetObservation("subaTANageDEC_len");
  map<unsigned, vector<obs::Comparison> >& comparisons = observation->comparisons();
  ASSERT_EQ(1u, comparisons.size()); // only one year of data
  // from CASAL model with same configuration
  vector<Double> expected_vals = {0.00518379,	0.0110955,	0.0145433,	0.0401723,	0.133707,	0.267919,	0.267355,	0.173347,	0.0866765};
  unsigned year = 1990;
  ASSERT_FALSE(comparisons.find(year) == comparisons.end());
  ASSERT_EQ(9, comparisons[year].size()); // 9 length bins
  EXPECT_EQ("male+female", comparisons[year][0].category_);
  for(unsigned i = 0; i < expected_vals.size(); ++i) {
    EXPECT_NEAR(expected_vals[i], comparisons[year][i].expected_, 0.0001);
  }
}

/**
 * Test error catching, when users supply length bins for this obs, which don't match the model lenght bins
 */
TEST_F(InternalEmptyModel, Observation_Proportions_At_Length_bespoke_length_bins_expect_error) {
  AddConfigurationLine(hak_like_model, __FILE__, 31);
  AddConfigurationLine(prop_at_length_expect_fail_incorrect_bespoke_length_bins, __FILE__, 31);
  LoadConfiguration();
  EXPECT_THROW(model_->Start(RunMode::kBasic), std::string);
}
/**
 * Test error catching, incorrect length bins in table supplied
 */
TEST_F(InternalEmptyModel, Observation_Proportions_At_Length_bespoke_length_bins_expect_error_wrong_table_input) {
  AddConfigurationLine(hak_like_model, __FILE__, 31);
  AddConfigurationLine(prop_at_length_expect_fail_incorrect_proportions_supplied, __FILE__, 31);
  LoadConfiguration();
  EXPECT_THROW(model_->Start(RunMode::kBasic), std::string);
}

}  // namespace age
} /* namespace niwa */

#endif /* TESTMODE */
