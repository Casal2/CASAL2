/**
 * @file Biomass.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 26/02/2014
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
namespace observations {

using std::cout;
using std::endl;
using niwa::testfixtures::InternalEmptyModel;


const std::string test_cases_observation_biomass =
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
ssb_offset 3
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
type biomass
catchability [type=free; q=6.52606e-005]
time_step one
categories male+female
selectivities male=[type=logistic; a50=9; ato95=4] female=[type=logistic; a50=9; ato95=4; alpha=0.7]
likelihood lognormal
years 1992:2001
obs 1.50 1.10 0.93 1.33 1.53 0.90 0.68 0.75 0.57 1.23
error_value 0.35
time_step_proportion 1.0
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Observation_Biomass) {
  AddConfigurationLine(test_cases_observation_biomass, __FILE__, 32);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(1494.5388146229072, obj_function.score());

  Observation* observation = model_->managers().observation()->GetObservation("observation");

  map<unsigned, vector<obs::Comparison> >& comparisons = observation->comparisons();
  ASSERT_EQ(10u, comparisons.size());

  unsigned year = 1992;
  ASSERT_FALSE(comparisons.find(year) == comparisons.end());
  ASSERT_EQ(1u, comparisons[year].size());
  EXPECT_EQ("male+female",              comparisons[year][0].category_);
  EXPECT_DOUBLE_EQ(0.35,                comparisons[year][0].error_value_);
  EXPECT_DOUBLE_EQ(383.22361249183439,  comparisons[year][0].expected_);
  EXPECT_DOUBLE_EQ(1.50,                comparisons[year][0].observed_);
  EXPECT_DOUBLE_EQ(129.11209792140789,  comparisons[year][0].score_);

  year = 1993;
  ASSERT_FALSE(comparisons.find(year) == comparisons.end());
  ASSERT_EQ(1u, comparisons[year].size());
  EXPECT_EQ("male+female",              comparisons[year][0].category_);
  EXPECT_DOUBLE_EQ(0.35,                comparisons[year][0].error_value_);
  EXPECT_DOUBLE_EQ(383.15138489379262,  comparisons[year][0].expected_);
  EXPECT_DOUBLE_EQ(1.1000000000000001,  comparisons[year][0].observed_);
  EXPECT_DOUBLE_EQ(144.24143898717551,  comparisons[year][0].score_);

  year = 1996;
  ASSERT_FALSE(comparisons.find(year) == comparisons.end());
  ASSERT_EQ(1u, comparisons[year].size());
  EXPECT_EQ("male+female",              comparisons[year][0].category_);
  EXPECT_DOUBLE_EQ(0.35,                comparisons[year][0].error_value_);
  EXPECT_DOUBLE_EQ(382.90136444491566,  comparisons[year][0].expected_);
  EXPECT_DOUBLE_EQ(1.53,                comparisons[year][0].observed_);
  EXPECT_DOUBLE_EQ(128.13400959067698,  comparisons[year][0].score_);

  year = 1998;
  ASSERT_FALSE(comparisons.find(year) == comparisons.end());
  ASSERT_EQ(1u, comparisons[year].size());
  EXPECT_EQ("male+female",              comparisons[year][0].category_);
  EXPECT_DOUBLE_EQ(0.35,                comparisons[year][0].error_value_);
  EXPECT_DOUBLE_EQ(382.78250454637208,  comparisons[year][0].expected_);
  EXPECT_DOUBLE_EQ(0.68000000000000005, comparisons[year][0].observed_);
  EXPECT_DOUBLE_EQ(169.31122485291806,  comparisons[year][0].score_);

  year = 2001;
  ASSERT_FALSE(comparisons.find(year) == comparisons.end());
  ASSERT_EQ(1u, comparisons[year].size());
  EXPECT_EQ("male+female",              comparisons[year][0].category_);
  EXPECT_DOUBLE_EQ(0.35,                comparisons[year][0].error_value_);
  EXPECT_DOUBLE_EQ(382.67564206567096,  comparisons[year][0].expected_);
  EXPECT_DOUBLE_EQ(1.23,                comparisons[year][0].observed_);
  EXPECT_DOUBLE_EQ(138.63228485336171,  comparisons[year][0].score_);
}

} /* namespace processes */
} /* namespace niwa */


#endif /* TESTMODE */
