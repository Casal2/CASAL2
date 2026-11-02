/**
 * @file TaggingLength.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2025/05/07
 * @section LICENSE
 *
 * Copyright Casal2 Project 2025 - https://github.com/Casal2/
 *
 * Length-based Tagging tests
 */
#ifdef TESTMODE

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

#include "Model/Managers.h"
#include "Partition/Partition.h"
#include "Processes/Manager.h"
#include "TestResources/TestFixtures/InternalEmptyLengthModel.h"

namespace niwa::processes::common {
using niwa::testfixtures::InternalEmptyLengthModel;

/**
 *
 */
const std::string length_mortality_tagging_basic =
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
names male female tagged
growth_increment growth_model*3

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
processes recruitment growth mortality tagging

@recruitment recruitment
type constant
categories male female
proportions 0.6 0.4
length_bins 1
r0 100000

@process mortality
type mortality_constant_rate
m 0.6 0.4
relative_m_by_length maturity
categories male female
time_step_proportions 1.0  

@process growth
type growth
categories male female

@selectivity maturity
type logistic
a50 3
ato95 1

@selectivity one
type constant
c 1
)";

/*
 * Simple unit test
 */
TEST_F(InternalEmptyLengthModel, Processes_Tagging_Length) {
  const string tagging_process = R"(
    @process tagging
    type tagging
    years 2000
    from male
    to tagged
    initial_mortality 0
    u_max 0.99
    selectivities one
    penalty none
    n 1
    table proportions
    year 1    2	   3    4    5 	
    2000 0	  0	   0    0    1 
    end_table
  )";

  AddConfigurationLine(length_mortality_tagging_basic, __FILE__, 29);
  AddConfigurationLine(tagging_process, __FILE__, 92);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  partition::Category& male = model_->partition().category("male");
  EXPECT_NEAR(2386.4029964424817, male.data_[0], 1e-5);
  EXPECT_NEAR(3518.5898218482757, male.data_[1], 1e-5);
  EXPECT_NEAR(4455.577433333201, male.data_[2], 1e-5);
  EXPECT_NEAR(5792.8574901255852, male.data_[3], 1e-5);
  EXPECT_NEAR(19715.342584124097, male.data_[4], 1e-5);

  partition::Category& female = model_->partition().category("female");
  EXPECT_NEAR(1594.7359808890521, female.data_[0], 1e-5);
  EXPECT_NEAR(2434.9257951819186, female.data_[1], 1e-5);
  EXPECT_NEAR(3495.1296110602852, female.data_[2], 1e-5);
  EXPECT_NEAR(4705.6997736982248, female.data_[3], 1e-5);
  EXPECT_NEAR(16053.928757326928, female.data_[4], 1e-5);

  partition::Category& tagged = model_->partition().category("tagged");
  EXPECT_NEAR(0, tagged.data_[0], 1e-5);
  EXPECT_NEAR(0, tagged.data_[1], 1e-5);
  EXPECT_NEAR(0, tagged.data_[2], 1e-5);
  EXPECT_NEAR(0, tagged.data_[3], 1e-5);
  EXPECT_NEAR(1.0, tagged.data_[4], 1e-5);
}

/*
 * min_age and max_age are optional and only the Age profile gives them a value. The process report
 * prints them, so on a Length model they must not hold whatever the parameter's default happened to be
 */
TEST_F(InternalEmptyLengthModel, Processes_Tagging_Length_UnsetAgesAreZero) {
  const string tagging_process = R"(
    @process tagging
    type tagging
    years 2000
    from male
    to tagged
    initial_mortality 0
    u_max 0.99
    selectivities one
    penalty none
    n 1
    table proportions
    year 1    2	   3    4    5 	
    2000 0	  0	   0    0    1 
    end_table
  )";

  AddConfigurationLine(length_mortality_tagging_basic, __FILE__, 29);
  AddConfigurationLine(tagging_process, __FILE__, 92);
  LoadConfiguration();
  model_->Start(RunMode::kBasic);

  auto& parameters = model_->managers()->process()->GetProcess("tagging")->parameters();
  EXPECT_EQ(vector<string>{"0"}, parameters.Get(PARAM_MIN_AGE)->current_values());
  EXPECT_EQ(vector<string>{"0"}, parameters.Get(PARAM_MAX_AGE)->current_values());
}

/*
 * Complicate the test a bit more
 */
TEST_F(InternalEmptyLengthModel, Processes_Tagging_Length_HigherN) {
  const string tagging_process = R"(
    @process tagging
    type tagging
    years 2000
    from male
    to tagged
    initial_mortality 0
    u_max 0.99
    selectivities one
    penalty none
    n 103
    table proportions
    year 1    2	   3    4    5 	
    2000 0	  0	   0    0    1 
    end_table
  )";

  AddConfigurationLine(length_mortality_tagging_basic, __FILE__, 29);
  AddConfigurationLine(tagging_process, __FILE__, 92);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  partition::Category& male = model_->partition().category("male");
  EXPECT_NEAR(2386.4029964424817, male.data_[0], 1e-5);
  EXPECT_NEAR(3518.5898218482757, male.data_[1], 1e-5);
  EXPECT_NEAR(4455.577433333201, male.data_[2], 1e-5);
  EXPECT_NEAR(5792.8574901255852, male.data_[3], 1e-5);
  EXPECT_NEAR(19613.342584124097, male.data_[4], 1e-5);

  partition::Category& female = model_->partition().category("female");
  EXPECT_NEAR(1594.7359808890521, female.data_[0], 1e-5);
  EXPECT_NEAR(2434.9257951819186, female.data_[1], 1e-5);
  EXPECT_NEAR(3495.1296110602852, female.data_[2], 1e-5);
  EXPECT_NEAR(4705.6997736982248, female.data_[3], 1e-5);
  EXPECT_NEAR(16053.928757326928, female.data_[4], 1e-5);

  partition::Category& tagged = model_->partition().category("tagged");
  EXPECT_NEAR(0, tagged.data_[0], 1e-5);
  EXPECT_NEAR(0, tagged.data_[1], 1e-5);
  EXPECT_NEAR(0, tagged.data_[2], 1e-5);
  EXPECT_NEAR(0, tagged.data_[3], 1e-5);
  EXPECT_NEAR(103.0, tagged.data_[4], 1e-5);
}

/*
 * Complicate the test a bit more with some variance of proportions and N
 */
TEST_F(InternalEmptyLengthModel, Processes_Tagging_Length_HigherN_and_proportions) {
  const string tagging_process = R"(
    @process tagging
    type tagging
    years 2000
    from male
    to tagged
    initial_mortality 0
    u_max 0.99
    selectivities one
    penalty none
    n 1000
    table proportions
    year 1    2	   3    4    5 	
    2000 0.1	0.2	 0.25 0.25 0.2 
    end_table
  )";

  AddConfigurationLine(length_mortality_tagging_basic, __FILE__, 29);
  AddConfigurationLine(tagging_process, __FILE__, 92);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  partition::Category& male = model_->partition().category("male");
  EXPECT_NEAR(2286.4029964424817, male.data_[0], 1e-5);
  EXPECT_NEAR(3318.5898218482757, male.data_[1], 1e-5);
  EXPECT_NEAR(4205.577433333201, male.data_[2], 1e-5);
  EXPECT_NEAR(5542.8574901255852, male.data_[3], 1e-5);
  EXPECT_NEAR(19516.342584124097, male.data_[4], 1e-5);

  partition::Category& female = model_->partition().category("female");
  EXPECT_NEAR(1594.7359808890521, female.data_[0], 1e-5);
  EXPECT_NEAR(2434.9257951819186, female.data_[1], 1e-5);
  EXPECT_NEAR(3495.1296110602852, female.data_[2], 1e-5);
  EXPECT_NEAR(4705.6997736982248, female.data_[3], 1e-5);
  EXPECT_NEAR(16053.928757326928, female.data_[4], 1e-5);

  partition::Category& tagged = model_->partition().category("tagged");
  EXPECT_NEAR(100, tagged.data_[0], 1e-5);
  EXPECT_NEAR(200, tagged.data_[1], 1e-5);
  EXPECT_NEAR(250, tagged.data_[2], 1e-5);
  EXPECT_NEAR(250, tagged.data_[3], 1e-5);
  EXPECT_NEAR(200, tagged.data_[4], 1e-5);
}

}  // namespace niwa::processes::common
#endif  // TESTMODE
