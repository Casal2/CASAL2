/**
 * @file AddressableTransformation.Test.cpp
 * @author  C.Marsh
 * @date 25/10/2021
 * @section LICENSE
 *
 * Copyright NIWA Science 2021 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include <gmock/gmock.h>
#include <gtest/gtest.h>
// headers
#include "../ObjectiveFunction/ObjectiveFunction.h"
#include "../TestResources/TestFixtures/InternalEmptyModel.h"

// Namespaces

namespace niwa {
namespace estimatetransformations {
using niwa::testfixtures::InternalEmptyModel;

/**
 *  Simple test model
 */
const std::string simple_base_model =

R"(
@model
start_year 1975
final_year 1977
min_age 1
max_age 30
age_plus true
base_weight_units tonnes
initialisation_phases iphase1
time_steps step1 step2 step3

@categories
format stock
names stock
age_lengths age_size

@initialisation_phase iphase1
type derived

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
b0 75000
standardise_years 1976:1978
recruitment_multipliers   	      1.00 1.02472	1.04658	
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
1975	80	111
1976	152	336
1977	74	1214
end_table

table method
method  	category 	selectivity 	u_max 	time_step 	penalty
FishingWest   	stock   	westFSel 	0.7 	step1 		CatchMustBeTaken1
FishingEest  	stock   	eastFSel 	0.7 	step1 		CatchMustBeTaken1
end_table

@derived_quantity biomass_t1
type biomass
time_step step1
categories stock
time_step_proportion 0.5
time_step_proportion_method weighted_sum
selectivities MaturationSel

@selectivity MaturationSel
type all_values_bounded
l 2
h 13
v 0.02	0.05	0.13	0.29	0.50	0.70	0.84	0.93	0.97	0.99	0.99	1.00

@selectivity westFSel
type double_normal
mu 6.44798
sigma_l  3.00327
sigma_r 7.88331
alpha 1.0

@selectivity eastFSel
type double_normal
mu 10.8129
sigma_l  3.50265
sigma_r 6.86988
alpha 1.0

@selectivity One
type constant
c 1

@age_length age_size
type schnute
by_length true
time_step_proportions 0.25 0.5 0.0
y1 24.5
y2 104.8
tau1 1
tau2 20
a 0.131
b 1.70
cv_first 0.1
length_weight size_weight3

@length_weight size_weight3
type basic
units kgs
a 2.0e-6
b 3.288
)";

const std::string avg_diff_fail = R"(
  @parameter_transformation ycs_avg_diff
  type average_difference
  parameters process[Recruitment].recruitment_multipliers{1976} process[Recruitment].recruitment_multipliers{1977}

  @estimate ycs_avg
  type uniform
  parameter parameter_transformation[ycs_avg_diff].average_parameter
  lower_bound 0
  upper_bound 20
)";

/*
TEST_F(InternalEmptyModel, AddressableTransformations_avg_diff_fail) {
  AddConfigurationLine(simple_base_model, __FILE__, 22);
  AddConfigurationLine(avg_diff_fail, __FILE__, 22);
  EXPECT_THROW(LoadConfiguration(), std::string);
}
*/

}
}
#endif
