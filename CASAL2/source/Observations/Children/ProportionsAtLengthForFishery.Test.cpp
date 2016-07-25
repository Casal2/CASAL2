/**
 * @file ProcessProportionsAtLengthForFishery.Test.cpp
 * @author  C.Marsh
 * @date 21/08/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "ProportionsAtLengthForFishery.h"

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

const std::string test_cases_observation_proportions_at_length_for_fishery_single =
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

@categories 
format stock 
names stock
age_lengths age_size 

@initialisation_phase iphase1
type iterative
years 100

@time_step init  
processes [type=nop]

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
standardise_ycs_years 1989 1990 1991 1992 1993 1994 1995 1996 
ycs_values      1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 
steepness 0.9
ssb biomass_t1
age 1
ssb_offset 1

@process Ageing
type ageing
categories stock

@process instant_mort
type mortality_instantaneous
m 0.19
time_step_ratio 0.42 0.25 0.33
selectivities One
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

table fisheries
fishery       category  selectivity u_max   time_step penalty
FishingWest   stock     westFSel    0.7     step1     none
FishingEest   stock     eastFSel    0.7     step1     none
end_table

@derived_quantity biomass_t1
type biomass
time_step step1
categories stock
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
type process_proportions_at_length_for_fishery
years 1991 1992 1993 1994 1995 
likelihood multinomial
time_step step1
fishery FishingEest
process instant_mort
categories stock
length_plus_group false
length_bins 0 20 40 60 80 110
delta 1e-5
table obs
1991    0.2   0.3     0.1     0.2     0.2 
1992    0.12  0.25    0.28    0.25    0.1 
1993    0.0   0.05    0.05    0.10    0.80  
1994    0.05  0.1     0.05    0.05    0.75  
1995    0.3   0.4     0.2     0.05    0.05  
end_table
table error_values
1991 25
1992 37
1993 31
1994 34
1995 22
end_table   
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Observation_Proportions_At_Length_for_fishery_Single) {
  AddConfigurationLine(test_cases_observation_proportions_at_length_for_fishery_single, __FILE__, 31);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(313.6635868924738, obj_function.score());

  Observation* observation = model_->managers().observation()->GetObservation("observation");

  map<unsigned, vector<obs::Comparison> >& comparisons = observation->comparisons();
  ASSERT_EQ(5u, comparisons.size());

  unsigned year = 1992;
  ASSERT_FALSE(comparisons.find(year) == comparisons.end());
  ASSERT_EQ(5u, comparisons[year].size());
  EXPECT_EQ("stock",                        comparisons[year][0].category_);
  EXPECT_DOUBLE_EQ(37,                      comparisons[year][0].error_value_);
  EXPECT_DOUBLE_EQ(1.2882479154946077e-008, comparisons[year][0].expected_);
  EXPECT_DOUBLE_EQ(0.12,                    comparisons[year][0].observed_);
  EXPECT_DOUBLE_EQ(58.053610343773592,      comparisons[year][0].score_);

  EXPECT_EQ("stock",                        comparisons[year][1].category_);
  EXPECT_DOUBLE_EQ(37,                      comparisons[year][1].error_value_);
  EXPECT_DOUBLE_EQ(0.023315666243312768,    comparisons[year][1].expected_);
  EXPECT_DOUBLE_EQ(0.25 ,                   comparisons[year][1].observed_);
  EXPECT_DOUBLE_EQ(48.135349075361262,      comparisons[year][1].score_);

  EXPECT_EQ("stock",                        comparisons[year][2].category_);
  EXPECT_DOUBLE_EQ(37,                      comparisons[year][2].error_value_);
  EXPECT_DOUBLE_EQ(0.084381382173839128,    comparisons[year][2].expected_);
  EXPECT_DOUBLE_EQ(0.28,                    comparisons[year][2].observed_);
  EXPECT_DOUBLE_EQ(41.571293069551452,      comparisons[year][2].score_);

  EXPECT_EQ("stock",                        comparisons[year][3].category_);
  EXPECT_DOUBLE_EQ(37,                      comparisons[year][3].error_value_);
  EXPECT_DOUBLE_EQ(0.59514507275357065,     comparisons[year][3].expected_);
  EXPECT_DOUBLE_EQ(0.25,                    comparisons[year][3].observed_);
  EXPECT_DOUBLE_EQ(18.168311942943394,      comparisons[year][3].score_);

  EXPECT_EQ("stock",                        comparisons[year][4].category_);
  EXPECT_DOUBLE_EQ(37,                      comparisons[year][4].error_value_);
  EXPECT_DOUBLE_EQ(0.29715786594679816,     comparisons[year][4].expected_);
  EXPECT_DOUBLE_EQ(0.1,                     comparisons[year][4].observed_);
  EXPECT_DOUBLE_EQ(7.2263246073288832,      comparisons[year][4].score_);
}



} /* namespace processes */
} /* namespace niwa */


#endif /* TESTMODE */
