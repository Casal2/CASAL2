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
#include "Process.h"

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


const std::string test_cases_observation_process_abundance =
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
processes halfm fishing 

@time_step two
processes halfm

@time_step three
processes recruitment ageing

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
type process_abundance
catchability [type=free; q=6.52606e-005]
delta 1e-10
time_step two
process halfm
categories male+female
selectivities male=[type=logistic; a50=9; ato95=4] female=[type=logistic; a50=9; ato95=4; alpha=0.7]
likelihood lognormal
years 1992:2001
obs 1.50 1.10 0.93 1.33 1.53 0.90 0.68 0.75 0.57 1.23
error_value 0.35
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Observation_Process_Abundance) {
  AddConfigurationLine(test_cases_observation_process_abundance, __FILE__, 32);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(104357.9915848384, obj_function.score());

  Observation* observation = model_->managers().observation()->GetObservation("observation");

  map<unsigned, vector<obs::Comparison> >& comparisons = observation->comparisons();
  ASSERT_EQ(10u, comparisons.size());

  unsigned year = 1992;
  ASSERT_FALSE(comparisons.find(year) == comparisons.end());
  ASSERT_EQ(1u, comparisons[year].size());
  EXPECT_EQ("male+female",              comparisons[year][0].category_);
  EXPECT_DOUBLE_EQ(0.35,                comparisons[year][0].error_value_);
  EXPECT_DOUBLE_EQ(-20.151984669493967, comparisons[year][0].expected_);
  EXPECT_DOUBLE_EQ(1.50,                comparisons[year][0].observed_);
  EXPECT_DOUBLE_EQ(10608.527215563498,  comparisons[year][0].score_);

  year = 1993;
  ASSERT_FALSE(comparisons.find(year) == comparisons.end());
  ASSERT_EQ(1u, comparisons[year].size());
  EXPECT_EQ("male+female",              comparisons[year][0].category_);
  EXPECT_DOUBLE_EQ(0.35,                comparisons[year][0].error_value_);
  EXPECT_DOUBLE_EQ(-20.148188402164081,  comparisons[year][0].expected_);
  EXPECT_DOUBLE_EQ(1.1000000000000001,  comparisons[year][0].observed_);
  EXPECT_DOUBLE_EQ(10475.957800116032,  comparisons[year][0].score_);

  year = 1996;
  ASSERT_FALSE(comparisons.find(year) == comparisons.end());
  ASSERT_EQ(1u, comparisons[year].size());
  EXPECT_EQ("male+female",              comparisons[year][0].category_);
  EXPECT_DOUBLE_EQ(0.35,                comparisons[year][0].error_value_);
  EXPECT_DOUBLE_EQ(-20.135041918475192,  comparisons[year][0].expected_);
  EXPECT_DOUBLE_EQ(1.53,                comparisons[year][0].observed_);
  EXPECT_DOUBLE_EQ(10616.654030879328,  comparisons[year][0].score_);

  year = 1998;
  ASSERT_FALSE(comparisons.find(year) == comparisons.end());
  ASSERT_EQ(1u, comparisons[year].size());
  EXPECT_EQ("male+female",              comparisons[year][0].category_);
  EXPECT_DOUBLE_EQ(0.35,                comparisons[year][0].error_value_);
  EXPECT_DOUBLE_EQ(-20.12878746564024,  comparisons[year][0].expected_);
  EXPECT_DOUBLE_EQ(0.68000000000000005, comparisons[year][0].observed_);
  EXPECT_DOUBLE_EQ(10271.741196680667,  comparisons[year][0].score_);

  year = 2001;
  ASSERT_FALSE(comparisons.find(year) == comparisons.end());
  ASSERT_EQ(1u, comparisons[year].size());
  EXPECT_EQ("male+female",              comparisons[year][0].category_);
  EXPECT_DOUBLE_EQ(0.35,                comparisons[year][0].error_value_);
  EXPECT_DOUBLE_EQ(-20.123154162672272,  comparisons[year][0].expected_);
  EXPECT_DOUBLE_EQ(1.23,                comparisons[year][0].observed_);
  EXPECT_DOUBLE_EQ(10523.04778953198,  comparisons[year][0].score_);
}

} /* namespace observations */
} /* namespace niwa */


#endif /* TESTMODE */
