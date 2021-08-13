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
time_step_ratio 0.42 0.25 0.33
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
  EXPECT_DOUBLE_EQ(1.3049322854316948e-008, comparisons[year][0].expected_);
  EXPECT_DOUBLE_EQ(0.12, comparisons[year][0].observed_);
  EXPECT_DOUBLE_EQ(58.053573280444361, comparisons[year][0].score_);

  EXPECT_EQ("stock", comparisons[year][1].category_);
  EXPECT_DOUBLE_EQ(37, comparisons[year][1].error_value_);
  EXPECT_DOUBLE_EQ(0.023617445143820383, comparisons[year][1].expected_);
  EXPECT_DOUBLE_EQ(0.25, comparisons[year][1].observed_);
  EXPECT_DOUBLE_EQ(48.016392832372198, comparisons[year][1].score_);

  EXPECT_EQ("stock", comparisons[year][2].category_);
  EXPECT_DOUBLE_EQ(37, comparisons[year][2].error_value_);
  EXPECT_DOUBLE_EQ(0.085102989012712807, comparisons[year][2].expected_);
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

const std::string test_cases_observation_proportions_at_length_double =
    R"(
@model
min_age 2
max_age 25
plus_group t
start_year 1975
final_year 2002
initialisation_phases phase1
time_steps one two three

@categories
format sex
names male female
age_lengths age_size_male age_size_female

@initialisation_phase phase1
years 200
exclude_processes fishing

@time_step one
processes halfm fishing halfm

@time_step two
processes recruitment

@time_step three
processes ageing

@derived_quantity ssb
type biomass
categories male female
selectivities male_maturity female_maturity
time_step one

@ageing ageing
categories male female

@recruitment recruitment
type beverton_holt
categories male female
proportions 0.5 0.5
r0 5e6
age 3
steepness 0.9
ycs_values 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00
ssb ssb
ycs_years 1972:1999
standardise_ycs_years 1973:1999

@mortality halfm
type constant_rate
categories male female
relative_m_by_age [type=constant; c=1] halfm.1
m 0.10 0.10

@mortality fishing
type event_biomass
categories male female
years 1975:2002
units kgs
catches 1191 1488 1288 2004 609 750 997 596 302 344 544 362 509 574 804 977 991 2454 2775 2898 4094 3760 3761 3673 3524 3700 3700 3700
U_max 0.9
selectivities observation.male observation.female
penalty event_mortality_penalty

@selectivity male_maturity
type logistic
a50 5
ato95 2

@selectivity female_maturity
type logistic
a50 5
ato95 2

@selectivity one
type constant
c 1

@age_length age_size_male
type von_bertalanffy
length_weight [type=none]
by_length true
k 0.277
t0 0.11
linf 90.3

@age_length age_size_female
type von_bertalanffy
length_weight [type=none]
by_length true
k 0.202
t0 -0.20
linf 113.4

@penalty event_mortality_penalty
type process
log_scale True
multiplier 10

@observation observation
type proportions_at_age
likelihood lognormal
time_step one
categories male female
min_age 3
max_age 15
selectivities male=[type=logistic; a50=9; ato95=4] female=[type=logistic; a50=9; ato95=4; alpha=0.7]
years 1992
table obs
1992 0.0173 0.0193 0.0241 0.0346 0.0365 0.0657 0.0427 0.0667 0.0326 0.0307 0.0272 0.0141 0.0319 0.0353 0.0249 0.0146 0.0133 0.0547 0.0488 0.0745 0.0660 0.0750 0.0646 0.0304 0.0147 0.0399
end_table
table error_values
1992 1.091 0.770 0.539 0.421 0.412 0.297 0.367 0.322 0.391 0.510 0.523 0.734 0.481 0.612 0.643 0.756 0.772 0.399 0.369 0.331 0.306 0.304 0.309 0.461 0.752 0.423
end_table
time_step_proportion 1.0

@report DQ
type derived_quantity
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Observation_Proportions_At_Length_Double) {
  //  AddConfigurationLine(test_cases_observation_proportions_at_length_double, __FILE__, 194);
  //  LoadConfiguration();
  //
  //  ModelPtr model = Model::Instance();
  //  model->Start(RunMode::kBasic);
  //
  //  ObservationPtr observation = observations::Manager::Instance().GetObservation("observation");
  //
  //  map<unsigned, vector<obs::Comparison> >& comparisons = observation->comparisons();
  //  ASSERT_EQ(1u, comparisons.size());
  //
  //  unsigned year = 1992;
  //  ASSERT_FALSE(comparisons.find(year) == comparisons.end());
  //  ASSERT_EQ(26u, comparisons[year].size());
  //  EXPECT_EQ("male",                       comparisons[year][0].category_);
  //  EXPECT_DOUBLE_EQ(1.091,                 comparisons[year][0].error_value_);
  //  EXPECT_DOUBLE_EQ(0.0041584607534975501, comparisons[year][0].expected_);
  //  EXPECT_DOUBLE_EQ(0.0173,                comparisons[year][0].observed_);
  //  EXPECT_DOUBLE_EQ(1.9851433023156135,    comparisons[year][0].score_);
  //
  //  EXPECT_EQ("male",                       comparisons[year][1].category_);
  //  EXPECT_DOUBLE_EQ(0.770,                 comparisons[year][1].error_value_);
  //  EXPECT_DOUBLE_EQ(0.0070171259680794212, comparisons[year][1].expected_);
  //  EXPECT_DOUBLE_EQ(0.0193,                comparisons[year][1].observed_);
  //  EXPECT_DOUBLE_EQ(1.2811829881419403,    comparisons[year][1].score_);
  //
  //  EXPECT_EQ("male",                       comparisons[year][2].category_);
  //  EXPECT_DOUBLE_EQ(0.539,                 comparisons[year][2].error_value_);
  //  EXPECT_DOUBLE_EQ(0.011682048742702372,  comparisons[year][2].expected_);
  //  EXPECT_DOUBLE_EQ(0.0241,                comparisons[year][2].observed_);
  //  EXPECT_DOUBLE_EQ(0.73886489332777494,   comparisons[year][2].score_);
  //
  //  EXPECT_EQ("male",                       comparisons[year][3].category_);
  //  EXPECT_DOUBLE_EQ(0.421,                 comparisons[year][3].error_value_);
  //  EXPECT_DOUBLE_EQ(0.018938077956547807,  comparisons[year][3].expected_);
  //  EXPECT_DOUBLE_EQ(0.0346,                comparisons[year][3].observed_);
  //  EXPECT_DOUBLE_EQ(0.52825882781144118,   comparisons[year][3].score_);
  //
  //  EXPECT_EQ("male",                       comparisons[year][4].category_);
  //  EXPECT_DOUBLE_EQ(0.412,                 comparisons[year][4].error_value_);
  //  EXPECT_DOUBLE_EQ(0.029222930328445654,  comparisons[year][4].expected_);
  //  EXPECT_DOUBLE_EQ(0.0365,                comparisons[year][4].observed_);
  //  EXPECT_DOUBLE_EQ(-0.63798390702860996,  comparisons[year][4].score_);
  //
  //  ObjectiveFunction& obj_function = ObjectiveFunction::Instance();
  //  EXPECT_DOUBLE_EQ(30.122778748284137, obj_function.score());
}

}  // namespace age
} /* namespace niwa */

#endif /* TESTMODE */
