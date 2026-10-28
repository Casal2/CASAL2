/**
 * @file MortalityEventBiomass.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2025/05/08
 * @section LICENSE
 *
 * Copyright Casal2 Project 2025 - https://github.com/Casal2/
 *
 */
#ifdef TESTMODE

#include "MortalityEventRemoval.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Model/Managers.h"
#include "Penalties/Manager.h"
#include "Processes/Manager.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"

namespace niwa::processes::age {
using niwa::testfixtures::InternalEmptyModel;

const std::string test_case_mortality_event_biomass =
    R"(
@model
start_year 2000
final_year 2002
min_age 1
max_age 5
base_weight_units kgs
age_plus f
time_steps step_one

@categories
format sex
names male female
age_lengths no_age_length*2

@age_length no_age_length
type none
length_weight [type=none]

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

@selectivity sel_male
type logistic
a50 11.99
ato95 5.25

@selectivity sel_female
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
TEST_F(InternalEmptyModel, Process_Mortality_Event_Biomass) {
  const string morality_process = R"(
    @mortality mortality
    type event_biomass
    categories male
    years 2000
    catches 555
    U_max 0.9
    selectivities one
  )";

  AddConfigurationLine(test_case_mortality_event_biomass, __FILE__, 24);
  AddConfigurationLine(morality_process, __FILE__, __LINE__);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  partition::Category& male = model_->partition().category("male");
  EXPECT_NEAR(60000, male.data_[0], 1e-5);
  EXPECT_NEAR(60000, male.data_[1], 1e-5);
  EXPECT_NEAR(59445, male.data_[2], 1e-5);
  EXPECT_NEAR(0.0, male.data_[3], 1e-5);
  EXPECT_NEAR(0.0, male.data_[4], 1e-5);

  partition::Category& female = model_->partition().category("female");
  EXPECT_NEAR(40000, female.data_[0], 1e-5);
  EXPECT_NEAR(40000, female.data_[1], 1e-5);
  EXPECT_NEAR(40000, female.data_[2], 1e-5);
  EXPECT_NEAR(0.0, female.data_[3], 1e-5);
  EXPECT_NEAR(0.0, female.data_[4], 1e-5);
}

/**
 * Test when we run it for multiple years
 */
TEST_F(InternalEmptyModel, Process_Mortality_Event_Biomass_Three_Years) {
  const string morality_process = R"(
    @mortality mortality
    type event_biomass
    categories male
    years 2000 2001 2002
    catches 555 444 333
    U_max 0.9
    selectivities one
  )";

  AddConfigurationLine(test_case_mortality_event_biomass, __FILE__, 24);
  AddConfigurationLine(morality_process, __FILE__, __LINE__);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  partition::Category& male = model_->partition().category("male");
  EXPECT_NEAR(59888.380511840718, male.data_[0], 1e-5);
  EXPECT_NEAR(59665.763902126979, male.data_[1], 1e-5);
  EXPECT_NEAR(59113.855586032303, male.data_[2], 1e-5);
  EXPECT_NEAR(0.0, male.data_[3], 1e-5);
  EXPECT_NEAR(0.0, male.data_[4], 1e-5);

  partition::Category& female = model_->partition().category("female");
  EXPECT_NEAR(40000, female.data_[0], 1e-5);
  EXPECT_NEAR(40000, female.data_[1], 1e-5);
  EXPECT_NEAR(40000, female.data_[2], 1e-5);
  EXPECT_NEAR(0.0, female.data_[3], 1e-5);
  EXPECT_NEAR(0.0, female.data_[4], 1e-5);
}

/**
 * Test when we run across sexes
 */
TEST_F(InternalEmptyModel, Process_Mortality_Event_Biomass_Across_Categories) {
  const string morality_process = R"(
    @mortality mortality
    type event_biomass
    categories male female
    years 2000 2001 2002
    catches 555 444 333
    U_max 0.9
    selectivities one one
  )";

  AddConfigurationLine(test_case_mortality_event_biomass, __FILE__, 24);
  AddConfigurationLine(morality_process, __FILE__, __LINE__);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  partition::Category& male = model_->partition().category("male");
  EXPECT_NEAR(59933.177481011771, male.data_[0], 1e-5);
  EXPECT_NEAR(59799.755581232035, male.data_[1], 1e-5);
  EXPECT_NEAR(59467.866937756196, male.data_[2], 1e-5);
  EXPECT_NEAR(0.0, male.data_[3], 1e-5);
  EXPECT_NEAR(0.0, male.data_[4], 1e-5);

  partition::Category& female = model_->partition().category("female");
  EXPECT_NEAR(39955.451654007848, female.data_[0], 1e-5);
  EXPECT_NEAR(39866.503720821354, female.data_[1], 1e-5);
  EXPECT_NEAR(39645.244625170795, female.data_[2], 1e-5);
  EXPECT_NEAR(0.0, female.data_[3], 1e-5);
  EXPECT_NEAR(0.0, female.data_[4], 1e-5);
}

/**
 * Catch far exceeds the (mean-weight-weighted, for biomass) vulnerable stock, so u_max binds.
 * Exercises the u_max cap and the penalty trigger, which no other test in this file reaches.
 * Expected values were captured from the pre-TBM-3 code.
 */
TEST_F(InternalEmptyModel, Process_Mortality_Event_Biomass_UMaxCap_Penalty) {
  const string config = R"(
@model
start_year 2000
final_year 2002
min_age 1
max_age 5
base_weight_units kgs
age_plus f
time_steps step_one

@categories
format sex
names male female
age_lengths vb_age_length*2

@age_length vb_age_length
type von_bertalanffy
by_length true
linf 55.7
k 0.14
t0 -0.82
cv_first 0.1
cv_last 0.1
length_weight weight_basic

@length_weight weight_basic
type basic
units kgs
a 0.007289
b 3.2055

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

@selectivity one
type constant
c 1

@penalty event_penalty
type process
log_scale false
multiplier 1

@mortality mortality
type event_biomass
categories male female
years 2000 2001 2002
catches 1000000 5000000 1000000
U_max 0.5
selectivities one one
penalty event_penalty
)";

  AddConfigurationLine(config, __FILE__, __LINE__);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  partition::Category& male = model_->partition().category("male");
  EXPECT_DOUBLE_EQ(54958.225337365904, male.data_[0]);
  EXPECT_DOUBLE_EQ(27479.112668682952, male.data_[1]);
  EXPECT_DOUBLE_EQ(16470.26582897954, male.data_[2]);
  EXPECT_DOUBLE_EQ(0.0, male.data_[3]);
  EXPECT_DOUBLE_EQ(0.0, male.data_[4]);

  partition::Category& female = model_->partition().category("female");
  EXPECT_DOUBLE_EQ(36638.816891577269, female.data_[0]);
  EXPECT_DOUBLE_EQ(0.0, female.data_[3]);
  EXPECT_DOUBLE_EQ(0.0, female.data_[4]);

  Penalty* penalty = model_->managers()->penalty()->GetPenalty("event_penalty");
  ASSERT_NE(penalty, nullptr);
  EXPECT_DOUBLE_EQ(1662928892061.4797, AS_DOUBLE(penalty->GetScore()));

  Process* process = model_->managers()->process()->GetProcess("mortality");
  ASSERT_NE(process, nullptr);
  ostringstream report;
  process->FillReportCache(report);
  EXPECT_EQ("years: 2000 2001 2002 \nactual_catches: 1e+06 3.71045e+06 1e+06 \nexploitation_rate: 0.400626 0.5 0.0840296 \n", report.str());
}

}  // namespace niwa::processes::age
#endif  // TESTMODE