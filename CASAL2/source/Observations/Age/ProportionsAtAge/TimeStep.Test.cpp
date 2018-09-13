/**
 * @file ProportionsAtAge.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 11/03/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
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

using std::cout;
using std::endl;
using niwa::testfixtures::InternalEmptyModel;

const std::string test_cases_observation_proportions_at_age_single =
R"(
@model
min_age 2
max_age 25
age_plus t
start_year 1975
final_year 2002
base_weight_units kgs
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
time_step_proportion 1.0
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
selectivities [type=constant; c=1] halfm.one
m 0.10 0.10

@mortality fishing
type event_biomass
categories male female
years 1975:2002
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
k 0.277
t0 0.11
linf 90.3

@age_length age_size_female
type von_bertalanffy
length_weight [type=none]
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
categories male+female
min_age 3
max_age 15
selectivities male=[type=logistic; a50=9; ato95=4] female=[type=logistic; a50=9; ato95=4; alpha=0.7]
years 1992
table obs
1992 0.0241 0.0473 0.0448 0.071 0.078 0.104 0.0672 0.1213 0.0869 0.111 0.0788 0.0436 0.122
end_table
table error_values   
1992 1.399 0.795 0.764 0.663 0.724 0.735 0.709 0.684 0.673 0.59 0.669 0.878 0.53
end_table
time_step_proportion 1.0 

@report DQ
type derived_quantity
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Observation_Proportions_At_Age_Single) {
  AddConfigurationLine(test_cases_observation_proportions_at_age_single, __FILE__, 31);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(242.70420164433611, obj_function.score());

  Observation* observation = model_->managers().observation()->GetObservation("observation");

  map<unsigned, vector<obs::Comparison> >& comparisons = observation->comparisons();
  ASSERT_EQ(1u, comparisons.size());

  unsigned year = 1992;
  ASSERT_FALSE(comparisons.find(year) == comparisons.end());
  ASSERT_EQ(13u, comparisons[year].size());
  EXPECT_EQ("male+female",                comparisons[year][0].category_);
  EXPECT_DOUBLE_EQ(1.399,                 comparisons[year][0].error_value_);
  EXPECT_DOUBLE_EQ(0, comparisons[year][0].expected_);
  EXPECT_DOUBLE_EQ(0.0241,                comparisons[year][0].observed_);
  EXPECT_DOUBLE_EQ(240.56840045905705,    comparisons[year][0].score_);

  EXPECT_EQ("male+female",              comparisons[year][1].category_);
  EXPECT_DOUBLE_EQ(0.79500000000000004, comparisons[year][1].error_value_);
  EXPECT_DOUBLE_EQ(0.012012717333534687,comparisons[year][1].expected_);
  EXPECT_DOUBLE_EQ(0.047300000000000002,comparisons[year][1].observed_);
  EXPECT_DOUBLE_EQ(2.307067628174821,  comparisons[year][1].score_);

  EXPECT_EQ("male+female",              comparisons[year][2].category_);
  EXPECT_DOUBLE_EQ(0.76400000000000001, comparisons[year][2].error_value_);
  EXPECT_DOUBLE_EQ(0.019998740797780443,comparisons[year][2].expected_);
  EXPECT_DOUBLE_EQ(0.0448,              comparisons[year][2].observed_);
  EXPECT_DOUBLE_EQ(0.77965326111568212, comparisons[year][2].score_);

  EXPECT_EQ("male+female",              comparisons[year][3].category_);
  EXPECT_DOUBLE_EQ(0.66300000000000003, comparisons[year][3].error_value_);
  EXPECT_DOUBLE_EQ(0.032420722865177096,comparisons[year][3].expected_);
  EXPECT_DOUBLE_EQ(0.070999999999999994,comparisons[year][3].observed_);
  EXPECT_DOUBLE_EQ(0.77591259580351335, comparisons[year][3].score_);

  EXPECT_EQ("male+female",              comparisons[year][4].category_);
  EXPECT_DOUBLE_EQ(0.72399999999999998, comparisons[year][4].error_value_);
  EXPECT_DOUBLE_EQ(0.050028329235357299,comparisons[year][4].expected_);
  EXPECT_DOUBLE_EQ(0.078,               comparisons[year][4].observed_);
  EXPECT_DOUBLE_EQ(0.076720938255509741,comparisons[year][4].score_);
}

const std::string test_cases_observation_proportions_at_age_double =
R"(
@model
min_age 2
max_age 25
age_plus t
start_year 1975
final_year 2002
base_weight_units kgs
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
time_step_proportion 1.0
selectivities male_maturity female_maturity
time_step one

@ageing ageing
categories male female

@recruitment recruitment
type beverton_holt
categories male female
proportions 0.5 0.5
r0 5e6
age 2
steepness 0.9
ycs_values 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00
ssb ssb
ycs_years 1973:2000
standardise_ycs_years 1973:1999

@mortality halfm
type constant_rate
categories male female
selectivities [type=constant; c=1] halfm.one
m 0.10 0.10

@mortality fishing
type event_biomass
categories male female
years 1975:2002
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
TEST_F(InternalEmptyModel, Observation_Proportions_At_Age_Double) {
  AddConfigurationLine(test_cases_observation_proportions_at_age_double, __FILE__, 194);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  Observation* observation = model_->managers().observation()->GetObservation("observation");

  map<unsigned, vector<obs::Comparison> >& comparisons = observation->comparisons();
  ASSERT_EQ(1u, comparisons.size());

  unsigned year = 1992;
  ASSERT_FALSE(comparisons.find(year) == comparisons.end());
  ASSERT_EQ(26u, comparisons[year].size());
  EXPECT_EQ("male",                       comparisons[year][0].category_);
  EXPECT_DOUBLE_EQ(1.091,                 comparisons[year][0].error_value_);
  EXPECT_DOUBLE_EQ(0.0041584600031340851, comparisons[year][0].expected_);
  EXPECT_DOUBLE_EQ(0.0173,                comparisons[year][0].observed_);
  EXPECT_DOUBLE_EQ(1.9851437206265514,    comparisons[year][0].score_);

  EXPECT_EQ("male",                       comparisons[year][1].category_);
  EXPECT_DOUBLE_EQ(0.770,                 comparisons[year][1].error_value_);
  EXPECT_DOUBLE_EQ(0.0070171274241356578, comparisons[year][1].expected_);
  EXPECT_DOUBLE_EQ(0.0193,                comparisons[year][1].observed_);
  EXPECT_DOUBLE_EQ(1.2811824334500412,    comparisons[year][1].score_);

  EXPECT_EQ("male",                       comparisons[year][2].category_);
  EXPECT_DOUBLE_EQ(0.539,                 comparisons[year][2].error_value_);
  EXPECT_DOUBLE_EQ(0.011682052659096514,  comparisons[year][2].expected_);
  EXPECT_DOUBLE_EQ(0.0241,                comparisons[year][2].observed_);
  EXPECT_DOUBLE_EQ(0.73886377382316992,   comparisons[year][2].score_);

  EXPECT_EQ("male",                       comparisons[year][3].category_);
  EXPECT_DOUBLE_EQ(0.421,                 comparisons[year][3].error_value_);
  EXPECT_DOUBLE_EQ(0.01893809046477354,  comparisons[year][3].expected_);
  EXPECT_DOUBLE_EQ(0.0346,                comparisons[year][3].observed_);
  EXPECT_DOUBLE_EQ(0.52825605809997567,   comparisons[year][3].score_);

  EXPECT_EQ("male",                       comparisons[year][4].category_);
  EXPECT_DOUBLE_EQ(0.412,                 comparisons[year][4].error_value_);
  EXPECT_DOUBLE_EQ(0.029222941304074518,  comparisons[year][4].expected_);
  EXPECT_DOUBLE_EQ(0.0365,                comparisons[year][4].observed_);
  EXPECT_DOUBLE_EQ(-0.63798462748572637,  comparisons[year][4].score_);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(30.122736447441941, obj_function.score());
}

} /* namespace processes */
} /* namespace niwa */


#endif /* TESTMODE */
