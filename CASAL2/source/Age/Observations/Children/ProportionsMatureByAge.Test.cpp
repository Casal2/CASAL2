
/**
 * @file ProportionsMatureByAge.Test.cpp
 * @author Craig Marsh
 * @github https://github.com/Zaita
 * @date 13/07/2017
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "ProportionsMatureByAge.h"

#include <iostream>

#include "Common/ObjectiveFunction/ObjectiveFunction.h"
#include "Common/Observations/Manager.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"

// Namespaces
namespace niwa {
namespace observations {

using std::cout;
using std::endl;
using niwa::testfixtures::InternalEmptyModel;

const std::string test_cases_observation_proportions_mature_by_age_age_single =
R"(
@model
start_year 1990
final_year 1994
min_age 1
max_age 100
age_plus true
base_weight_units tonnes
initialisation_phases Equilibrium_state
time_steps Annual

@categories 
format Maturity 
names nonspawn spawn
age_lengths VB VB

@initialisation_phase Equilibrium_state
type derived
casal_intialisation_switch false

@time_step Annual 
processes Ageing Recruitment Maturation Instantaneous_Mortality

@process Recruitment
type recruitment_beverton_holt
categories spawn nonspawn
proportions 0.0 1.0
b0 57570
ycs_years 1989:1993
standardise_ycs_years 1991:1993
ycs_values 1*5
steepness 0.75
ssb SSB
age 1

@process Ageing
type ageing
categories *

@process Maturation
type transition_category
from nonspawn 
to spawn
selectivities MaturationSel
proportions 1

@process Instantaneous_Mortality
type mortality_instantaneous
m 0.045
time_step_ratio 1
selectivities One
categories *
table catches
year NWCR
1991 12180
1992 13300
1993 4180
1994 3850
end_table

table method
method    category  selectivity   u_max   time_step   penalty
NWCR      spawn       NWCR_Fsel   0.67    Annual    none
end_table

@derived_quantity SSB
type biomass
time_step Annual
categories spawn
time_step_proportion 0.75
time_step_proportion_method weighted_sum
selectivities One

@selectivity MaturationSel
type logistic_producing
length_based false
l 10 
h 60
a50 37 
ato95 4.56

@selectivity NWCR_Fsel 
type constant
c 1

@selectivity Trawlsel
type logistic
a50 6
ato95 3

@selectivity One
type constant 
c 1

@age_length VB
type von_bertalanffy
by_length false
time_step_proportions 0.0
k 0.059
t0 -0.491
Linf 37.78
cv_first 0.09483
cv_last 0.04498
distribution normal
length_weight wgt 

@length_weight wgt
type basic
units tonnes 
a 8.0e-8
b 2.75

@catchability acoq_99_12
type free
q 0.7685


@observation aco_99_12
type biomass 
time_step Annual
time_step_proportion 0.75
categories spawn
selectivities NWCR_Fsel 
catchability acoq_99_12
years     1992 1993
obs     8126 14637
error_value   0.22 0.09 
likelihood lognormal

@observation LFcom
type proportions_at_length
time_step Annual
time_step_proportion 0.5
length_bins 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47
length_plus_group true
categories spawn 
selectivities NWCR_Fsel
delta 0.00001
years 1993 
likelihood multinomial

table obs
1993 0 0 0 0 0 0 0 0 9.20e-05 9.20e-05 0 0.000109156 0.000543102 0.00193728 0.001826512 0.006905244 0.005842306 0.012439678 0.024062154 0.03569912 0.059544832 0.076954497 0.113241442 0.127653548 0.136083093 0.131562826 0.111258205 0.081511835 0.043014651 0.018749065 0.007108662 0.002293693 0.000655585 0.000655585 0.000163896 0
end_table

table error_values
1993 19
end_table

@observation Mature_1994
type proportions_mature_by_age
time_step Annual
time_step_proportion 0.5
min_age 10
max_age 89
age_plus false
categories spawn 
total_categories nonspawn
delta 0.00001
years 1994
likelihood binomial

table obs
1994 0.00000000 0.00000000 0.00000000 0.00000000 0.00000000 0.00000000 0.00000000 0.28571430 0.00000000 0.00000000 0.00000000 0.00000000 0.00000000 0.20000000 0.00000000 0.00000000 0.00000000 0.07692308 0.00000000 0.00000000 0.11111110 0.08333333 0.50000000 0.20000000 0.16666670 0.50000000 0.75000000 0.50000000 0.20000000 1.00000000 0.80000000 0.00000000 0.00000000 0.25000000 0.80000000 1.00000000 1.00000000 0.00000000 0.00000000 1.00000000 0.00000000 0.00000000 1.00000000 0.00000000 1.00000000 1.00000000 1.00000000 1.00000000 0.00000000 0.66666670 0.00000000 1.00000000 1.00000000 1.00000000 1.00000000 0.00000000 0.00000000 0.00000000 0.00000000 1.00000000 0.00000000 1.00000000 0.00000000 0.00000000 0.00000000 0.00000000 0.00000000 1.00000000 0.00000000 0.00000000 0.00000000 1.00000000 0.66666670 0.00000000 1.00000000 0.00000000 0.00000000 1.00000000 1.00000000 0.50000000
end_table

table error_values
1994 0 5 3 4 0 4 8 0 9 1 5 8 7 5 7 5 2 13 8 4 9 12 4 5 6 2 4 6 5 3 5 2 2 4 5 3 1 1 0 1 0 0 1 1 3 1 1 1 1 3 0 2 1 1 2 0 0 0 0 4 0 2 0 0 0 0 0 1 0 0 0 3 0 0 1 0 0 1 1 0
end_table

)";

/**
 *
 */
TEST_F(InternalEmptyModel, Observation_Proportions_Mature_By_Age) {
  AddConfigurationLine(test_cases_observation_proportions_mature_by_age_age_single, __FILE__, 31);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(130.65624641705122, obj_function.score());

  Observation* observation = model_->managers().observation()->GetObservation("Mature_1994");

  map<unsigned, vector<obs::Comparison> >& comparisons = observation->comparisons();
  ASSERT_EQ(1u, comparisons.size());

  unsigned year = 1994;
  ASSERT_FALSE(comparisons.find(year) == comparisons.end());
  ASSERT_EQ(80u, comparisons[year].size());
  EXPECT_EQ("spawn",                      comparisons[year][0].category_);
  EXPECT_DOUBLE_EQ(0,                     comparisons[year][0].error_value_);
  EXPECT_DOUBLE_EQ(2.5188981846777863e-008,comparisons[year][0].expected_);
  EXPECT_DOUBLE_EQ(0,                     comparisons[year][0].observed_);
  EXPECT_DOUBLE_EQ(0,                     comparisons[year][0].score_);

  EXPECT_EQ("spawn",                      comparisons[year][10].category_);
  EXPECT_DOUBLE_EQ(5,                     comparisons[year][10].error_value_);
  EXPECT_DOUBLE_EQ(1.360804112592696e-005,comparisons[year][10].expected_);
  EXPECT_DOUBLE_EQ(0,                     comparisons[year][10].observed_);
  EXPECT_DOUBLE_EQ(6.8040668580829513e-005,comparisons[year][10].score_);

  EXPECT_EQ("spawn",                      comparisons[year][20].category_);
  EXPECT_DOUBLE_EQ(9,                     comparisons[year][20].error_value_);
  EXPECT_DOUBLE_EQ(0.0085862448944578409, comparisons[year][20].expected_);
  EXPECT_DOUBLE_EQ(0.1111111,             comparisons[year][20].observed_);
  EXPECT_DOUBLE_EQ(2.629355459137424,     comparisons[year][20].score_);

  EXPECT_EQ("spawn",                      comparisons[year][30].category_);
  EXPECT_DOUBLE_EQ(5,                     comparisons[year][30].error_value_);
  EXPECT_DOUBLE_EQ(0.8032830009199865,    comparisons[year][30].expected_);
  EXPECT_DOUBLE_EQ(0.80000000000000004,   comparisons[year][30].observed_);
  EXPECT_DOUBLE_EQ(0.8927440138742192,    comparisons[year][30].score_);

  EXPECT_EQ("spawn",                      comparisons[year][70].category_);
  EXPECT_DOUBLE_EQ(0,                     comparisons[year][70].error_value_);
  EXPECT_DOUBLE_EQ(1,                     comparisons[year][70].expected_);
  EXPECT_DOUBLE_EQ(0,                     comparisons[year][70].observed_);
  EXPECT_DOUBLE_EQ(0,                     comparisons[year][70].score_);
}

} /* namespace processes */
} /* namespace niwa */


#endif /* TESTMODE */
