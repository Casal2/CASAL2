/**
 * @file MortalityConstantRate.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2025/05/07
 * @section LICENSE
 *
 * Copyright Casal2 Project 2025 - https://github.com/Casal2/
 *
 */
#ifdef TESTMODE

#include "MortalityConstantRemovalRate.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

#include "TestResources/TestFixtures/InternalEmptyLengthModel.h"

namespace niwa::processes::length {
using niwa::testfixtures::InternalEmptyLengthModel;

/**
 *
 */
const std::string length_mortality_constant_rate_basic =
    R"(
@model
type length
start_year 2000 
final_year 2000
length_bins  1:5
length_plus true
length_plus_group 6
base_weight_units tonnes
time_steps one

@categories
format sex
names male female
growth_increment growth_model*2

@growth_increment growth_model
type basic
l_alpha 1
l_beta  3
g_alpha 5
g_beta 1
min_sigma 2
distribution normal
length_weight [type=none]
cv 0.1
compatibility_option casal2

@time_step one 
processes recruitment growth mortality

@recruitment recruitment
type constant
categories *
proportions 0.6 0.4
length_bins 1
r0 100000

@process growth
type growth
categories *

@selectivity maturity
type logistic
a50 3
ato95 1
)";

/*
 * Sample unit test
 */
TEST_F(InternalEmptyLengthModel, Processes_Mortality_ConstantRate) {
  const string mortality_process = R"(
    @process mortality
    type mortality_constant_rate
    m 0.6 0.4
    relative_m_by_length maturity
    categories male female
    time_step_proportions 1.0  
  )";

  AddConfigurationLine(length_mortality_constant_rate_basic, __FILE__, 29);
  AddConfigurationLine(mortality_process, __FILE__, 81);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  partition::Category& male = model_->partition().category("male");
  EXPECT_NEAR(2386.4029964424817, male.data_[0], 1e-5);
  EXPECT_NEAR(3518.5898218482757, male.data_[1], 1e-5);
  EXPECT_NEAR(4455.577433333201, male.data_[2], 1e-5);
  EXPECT_NEAR(5792.8574901255852, male.data_[3], 1e-5);
  EXPECT_NEAR(19716.342584124097, male.data_[4], 1e-5);

  partition::Category& female = model_->partition().category("female");
  EXPECT_NEAR(1594.7359808890521, female.data_[0], 1e-5);
  EXPECT_NEAR(2434.9257951819186, female.data_[1], 1e-5);
  EXPECT_NEAR(3495.1296110602852, female.data_[2], 1e-5);
  EXPECT_NEAR(4705.6997736982248, female.data_[3], 1e-5);
  EXPECT_NEAR(16053.928757326928, female.data_[4], 1e-5);
}

/**
 *
 */
const std::string length_mortality_constant_rate_basic_three_years =
    R"(
@model
type length
start_year 2000 
final_year 2002
length_bins  1:5
length_plus true
length_plus_group 6
base_weight_units tonnes
time_steps one

@categories
format sex
names male female
growth_increment growth_model*2

@growth_increment growth_model
type basic
l_alpha 1
l_beta  3
g_alpha 5
g_beta 1
min_sigma 2
distribution normal
length_weight [type=none]
cv 0.1
compatibility_option casal2

@time_step one 
processes recruitment growth mortality

@recruitment recruitment
type constant
categories *
proportions 0.6 0.4
length_bins 1
r0 100000

@process growth
type growth
categories *

@selectivity maturity
type logistic
a50 3
ato95 1
)";

/*
 * Sample unit test
 */
TEST_F(InternalEmptyLengthModel, Processes_Mortality_ConstantRate_ThreeYears) {
  const string mortality_process = R"(
    @process mortality
    type mortality_constant_rate
    m 0.6 0.4
    relative_m_by_length maturity
    categories male female
    time_step_proportions 1.0  
  )";

  AddConfigurationLine(length_mortality_constant_rate_basic_three_years, __FILE__, 29);
  AddConfigurationLine(mortality_process, __FILE__, 81);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  partition::Category& male = model_->partition().category("male");
  EXPECT_NEAR(2485.0934209079692, male.data_[0], 1e-5);
  EXPECT_NEAR(4549.8614408188268, male.data_[1], 1e-5);
  EXPECT_NEAR(7551.506450677025, male.data_[2], 1e-5);
  EXPECT_NEAR(11947.044967632679, male.data_[3], 1e-5);
  EXPECT_NEAR(40989.25905838317, male.data_[4], 1e-5);

  partition::Category& female = model_->partition().category("female");
  EXPECT_NEAR(1660.8503678543993, female.data_[0], 1e-5);
  EXPECT_NEAR(3176.1368426009103, female.data_[1], 1e-5);
  EXPECT_NEAR(6382.1093044685749, female.data_[2], 1e-5);
  EXPECT_NEAR(11044.781264554316, female.data_[3], 1e-5);
  EXPECT_NEAR(38389.953073505232, female.data_[4], 1e-5);
}

}  // namespace niwa::processes::length
#endif  // TESTMODE