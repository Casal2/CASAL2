/**
 * @file Process.Test.cpp
 * @author Craig Marsh
 * @github https://github.com/Craig44
 * @date 2021
 * @section LICENSE
 *
 * Copyright NIWA Science �2016 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// headers
#include "../TestResources/TestFixtures/InternalEmptyModel.h"

namespace niwa {

using niwa::testfixtures::InternalEmptyModel;

const string replicate_category_recruitment =
    R"(
@model
start_year 1994
final_year 1995
min_age 1
base_weight_units kgs
max_age 20
age_plus t
initialisation_phases iphase1
time_steps step_one

@categories
format stage.sex
names immature.male mature.male immature.female mature.female
age_lengths no_age_length*4


@age_length no_age_length
type none
length_weight no_length_weight

@length_weight no_length_weight
type none

@initialisation_phase iphase1
years 20
convergence_years 20


@time_step step_one
processes Recruitment maturation halfM Recruitment_again

@time_step step_two
processes my_ageing

@ageing my_ageing
categories *

@Recruitment Recruitment
type constant
categories stage=immature
proportions 0.5 0.5
R0 997386
age 1

@Recruitment Recruitment_again
type constant
categories stage=immature
proportions 0.5 0.5
R0 997386
age 1

@mortality halfM
type constant_rate
categories *
M 0.065 0.065 0.065 0.065
relative_m_by_age One One One One

@process maturation
type transition_category
from stage=immature
to stage=mature
proportions 1.0 1.0
selectivities Maturation Maturation

@selectivity One
type constant
c 1

@selectivity Maturation
type logistic_producing
L 5
H 30
a50 8
ato95 3

@derived_quantity abundance
type abundance
categories *
selectivities Maturation Maturation Maturation Maturation
time_step step_one
)";

/**
 *  Test LogNormal @project for estimate of type map and
 */
TEST_F(InternalEmptyModel, Process_Verify_errors_with_category_in_multiple_recruit_blocks) {
  AddConfigurationLine(replicate_category_recruitment, __FILE__, 30);
  LoadConfiguration();
  EXPECT_THROW(model_->Start(RunMode::kBasic), std::string);
}
} /* namespace niwa */
#endif
