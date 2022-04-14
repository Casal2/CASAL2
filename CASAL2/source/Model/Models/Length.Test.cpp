/**
 * @file Length.Test.cpp
 * @author C.Marsh
 * @date 2020
 * @section LICENSE
 *
 * Copyright NIWA Science 2020 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// headers
#include "../../TestResources/TestFixtures/InternalEmptyModel.h"
#include "../../BaseClasses/Object.h"
#include "../../Model/Model.h"
#include "../../Model/Objects.h"
#include "../../Model/Objects.Mock.h"

namespace niwa {
namespace model {
using niwa::testfixtures::InternalEmptyModel;

/*
* This first set of tests check bounds sanity checks and also
* checks the simplex work on string maps using process[Recruitment].proportions parameter must sum = 1
*/
const string core_model =
R"(
@categories 
format sex
names uni
growth_increment growth_model

@initialisation_phase Equilibrium_state
type iterative
years 2

@time_step Annual 
processes  Nop

@process Nop
type null_process

@growth_increment growth_model
type basic
time_step_proportions 1
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
)";

const string length_model_with_min_age =
R"(
@model
type length
start_year 1986 
final_year 1987
length_bins  1:43
length_plus t
length_plus_group 45
base_weight_units tonnes
initialisation_phases Equilibrium_state
time_steps Annual
min_age 1
)";
const string length_model_with_max_age =
R"(
@model
type length
start_year 1986 
final_year 1987
length_bins  1:43
length_plus t
length_plus_group 45
base_weight_units tonnes
initialisation_phases Equilibrium_state
time_steps Annual
max_age 1
)";

/**
 * if users supply min_age Casal2 will stop
 */
TEST_F(InternalEmptyModel, length_model_with_min_age) {
  AddConfigurationLine(length_model_with_max_age, __FILE__, 51);
  AddConfigurationLine(core_model, __FILE__, 51);
  LoadConfiguration();
  EXPECT_THROW(model_->Start(RunMode::kBasic), std::string);
}
/**
 * if users supply min_age Casal2 will stop
 */
TEST_F(InternalEmptyModel, length_model_with_max_age) {
  AddConfigurationLine(length_model_with_min_age, __FILE__, 51);
  AddConfigurationLine(core_model, __FILE__, 51);
  LoadConfiguration();
  EXPECT_THROW(model_->Start(RunMode::kBasic), std::string);
}

} /* namespace model */
} /* namespace niwa */
#endif
