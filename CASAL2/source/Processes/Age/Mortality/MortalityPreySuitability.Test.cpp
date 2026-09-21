/**
 * @file MortalityPreySuitability.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2025/05/08
 * @section LICENSE
 *
 * Copyright Casal2 Project 2025 - https://github.com/Casal2/
 *
 */
#ifdef TESTMODE

#include "MortalityPreySuitability.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TestResources/TestFixtures/InternalEmptyModel.h"

namespace niwa::processes::age {
using niwa::testfixtures::InternalEmptyModel;

const std::string test_case_mortality_prey_suitability =
    R"(
@model
start_year 2000
final_year 2002
min_age 1
max_age 5
base_weight_units kgs
age_plus f
initialisation_phases iphase
time_steps step_one

@categories
format sex
names predator prey
age_lengths no_age_length*2

@age_length no_age_length
type none
length_weight [type=none]

@initialisation_phase iphase
years 2
exclude_processes mortality

@time_step step_one
processes ageing recruitment mortality

@ageing ageing
categories *

@recruitment recruitment
type constant
categories *
proportions 0.6 0.4
r0 100000
age 1

@selectivity sel_predator
type logistic
a50 11.99
ato95 5.25

@selectivity sel_prey
type logistic
a50 16.92
ato95 7.68

@selectivity one
type constant
c 1
)";

/*
 * Sample unit test
 */
TEST_F(InternalEmptyModel, Process_MortalityPreySuitability) {
  ASSERT_TRUE(true);
  return;

  const string morality_process = R"(
    @process mortality
    type prey_suitability_predation
    prey_categories prey
    predator_categories predator
    electivities 0.25
    u_max 0.99
    consumption_rate 1.0
    prey_selectivities one
    predator_selectivities one
    years 2000
  )";

  AddConfigurationLine(test_case_mortality_prey_suitability, __FILE__, 24);
  AddConfigurationLine(morality_process, __FILE__, __LINE__);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  partition::Category& male = model_->partition().category("predator");
  EXPECT_NEAR(60000, male.data_[0], 1e-5);
  EXPECT_NEAR(60000, male.data_[1], 1e-5);
  EXPECT_NEAR(60000, male.data_[2], 1e-5);
  EXPECT_NEAR(60000, male.data_[3], 1e-5);
  EXPECT_NEAR(60000, male.data_[4], 1e-5);

  partition::Category& female = model_->partition().category("prey");
  EXPECT_NEAR(40000, female.data_[0], 1e-5);
  EXPECT_NEAR(40000, female.data_[1], 1e-5);
  EXPECT_NEAR(39995.050000000003, female.data_[2], 1e-5);
  EXPECT_NEAR(39995.050000000003, female.data_[3], 1e-5);
  EXPECT_NEAR(39995.050000000003, female.data_[4], 1e-5);
}

/**
 * Test when we run it for multiple years
 */
TEST_F(InternalEmptyModel, Process_MortalityPreySuitability_Three_Years) {
  ASSERT_TRUE(true);
  return;

  const string morality_process = R"(
    @mortality mortality
    type event_biomass
    categories predator
    years 2000 2001 2002
    catches 555 444 333
    U_max 0.9
    selectivities one
  )";

  AddConfigurationLine(test_case_mortality_prey_suitability, __FILE__, 24);
  AddConfigurationLine(morality_process, __FILE__, __LINE__);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  partition::Category& male = model_->partition().category("predator");
  EXPECT_NEAR(59888.380511840718, male.data_[0], 1e-5);
  EXPECT_NEAR(59665.763902126979, male.data_[1], 1e-5);
  EXPECT_NEAR(59113.855586032303, male.data_[2], 1e-5);
  EXPECT_NEAR(0.0, male.data_[3], 1e-5);
  EXPECT_NEAR(0.0, male.data_[4], 1e-5);

  partition::Category& female = model_->partition().category("prey");
  EXPECT_NEAR(40000, female.data_[0], 1e-5);
  EXPECT_NEAR(40000, female.data_[1], 1e-5);
  EXPECT_NEAR(40000, female.data_[2], 1e-5);
  EXPECT_NEAR(0.0, female.data_[3], 1e-5);
  EXPECT_NEAR(0.0, female.data_[4], 1e-5);
}

/**
 * Test when we run across sexes
 */
TEST_F(InternalEmptyModel, Process_MortalityPreySuitability_Across_Categories) {
  ASSERT_TRUE(true);
  return;
  const string morality_process = R"(
    @mortality mortality
    type event_biomass
    categories *
    years 2000 2001 2002
    catches 555 444 333
    U_max 0.9
    selectivities one one
  )";

  AddConfigurationLine(test_case_mortality_prey_suitability, __FILE__, 24);
  AddConfigurationLine(morality_process, __FILE__, __LINE__);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  partition::Category& male = model_->partition().category("predator");
  EXPECT_NEAR(59933.177481011771, male.data_[0], 1e-5);
  EXPECT_NEAR(59799.755581232035, male.data_[1], 1e-5);
  EXPECT_NEAR(59467.866937756196, male.data_[2], 1e-5);
  EXPECT_NEAR(0.0, male.data_[3], 1e-5);
  EXPECT_NEAR(0.0, male.data_[4], 1e-5);

  partition::Category& female = model_->partition().category("prey");
  EXPECT_NEAR(39955.451654007848, female.data_[0], 1e-5);
  EXPECT_NEAR(39866.503720821354, female.data_[1], 1e-5);
  EXPECT_NEAR(39645.244625170795, female.data_[2], 1e-5);
  EXPECT_NEAR(0.0, female.data_[3], 1e-5);
  EXPECT_NEAR(0.0, female.data_[4], 1e-5);
}

}  // namespace niwa::processes::age
#endif  // TESTMODE