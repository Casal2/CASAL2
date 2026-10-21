/**
 * @file SurvivalConstantRate_Age.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2025/03/13
 * @section LICENSE
 *
 * Copyright Casal2 Project 2025 - https://github.com/Casal2/
 *
 */
#ifdef TESTMODE

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>

#include "Model/Factory.h"
#include "Model/Managers.h"
#include "Model/Model.h"
#include "Partition/Partition.h"
#include "Selectivities/Manager.h"
#include "Selectivities/Selectivity.h"
#include "TestResources/TestFixtures/BasicModel.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"
#include "TimeSteps/Manager.h"

namespace niwa::processes::age {
using niwa::testfixtures::BasicModel;
using niwa::testfixtures::InternalEmptyModel;
using std::cout;
using std::endl;

/**
 *
 */
TEST_F(BasicModel, Processes_Mortality_Survival_Rate) {
  // Recruitment process
  vector<string> recruitment_categories = {"immature.male", "immature.female"};
  vector<string> proportions            = {"0.6", "0.4"};
  base::Object*  process                = model_->factory().CreateObject(PARAM_RECRUITMENT, PARAM_CONSTANT);
  process->parameters().Add(PARAM_LABEL, "recruitment", __FILE__, __LINE__);
  process->parameters().Add(PARAM_TYPE, "constant", __FILE__, __LINE__);
  process->parameters().Add(PARAM_CATEGORIES, recruitment_categories, __FILE__, __LINE__);
  process->parameters().Add(PARAM_PROPORTIONS, proportions, __FILE__, __LINE__);
  process->parameters().Add(PARAM_R0, "100000", __FILE__, __LINE__);
  process->parameters().Add(PARAM_AGE, "1", __FILE__, __LINE__);

  // Report process
  base::Object* report = model_->factory().CreateObject(PARAM_REPORT, PARAM_DERIVED_QUANTITY);
  ASSERT_NE(report, nullptr);
  report->parameters().Add(PARAM_LABEL, "DQ", __FILE__, __LINE__);
  report->parameters().Add(PARAM_TYPE, "derived_quantity", __FILE__, __LINE__);

  // Mortality process
  vector<string> mortality_categories = {"immature.male", "immature.female", "mature.male", "mature.female"};
  process                             = model_->factory().CreateObject(PARAM_PROCESS, PARAM_SURVIVAL_CONSTANT_RATE);
  process->parameters().Add(PARAM_LABEL, "mortality", __FILE__, __LINE__);
  process->parameters().Add(PARAM_TYPE, "mortality_survival_constant_rate", __FILE__, __LINE__);
  process->parameters().Add(PARAM_CATEGORIES, mortality_categories, __FILE__, __LINE__);
  process->parameters().Add(PARAM_S, "0.935", __FILE__, __LINE__);
  process->parameters().Add(PARAM_SELECTIVITIES, "logistic", __FILE__, __LINE__);
  process->parameters().Add(PARAM_TIME_STEP_PROPORTIONS, "1.0", __FILE__, __LINE__);

  // Add a selectivity
  base::Object* selectivity = model_->factory().CreateObject(PARAM_SELECTIVITY, PARAM_LOGISTIC);
  selectivity->parameters().Add(PARAM_LABEL, "logistic", __FILE__, __LINE__);
  selectivity->parameters().Add(PARAM_TYPE, "logistic", __FILE__, __LINE__);
  selectivity->parameters().Add(PARAM_A50, "8", __FILE__, __LINE__);
  selectivity->parameters().Add(PARAM_ATO95, "3", __FILE__, __LINE__);

  // Ageing process
  vector<string> ageing_categories = {"immature.male", "immature.female"};
  process                          = model_->factory().CreateObject(PARAM_AGEING, "");
  process->parameters().Add(PARAM_LABEL, "ageing", __FILE__, __LINE__);
  process->parameters().Add(PARAM_CATEGORIES, ageing_categories, __FILE__, __LINE__);

  // Timestep
  base::Object*  time_step = model_->factory().CreateObject(PARAM_TIME_STEP, "");
  vector<string> processes = {"ageing", "recruitment", "mortality"};
  time_step->parameters().Add(PARAM_LABEL, "step_one", __FILE__, __LINE__);
  time_step->parameters().Add(PARAM_PROCESSES, processes, __FILE__, __LINE__);

  model_->Start(RunMode::kTesting);

  partition::Category& immature_male   = model_->partition().category("immature.male");
  partition::Category& immature_female = model_->partition().category("immature.female");
  partition::Category& mature_male     = model_->partition().category("mature.male");
  partition::Category& mature_female   = model_->partition().category("mature.female");

  // Verify blank partition
  for (unsigned i = 0; i < immature_male.data_.size(); ++i) EXPECT_DOUBLE_EQ(0.0, immature_male.data_[i]) << " where i = " << i << "; age = " << i + immature_male.min_age_;
  for (unsigned i = 0; i < immature_female.data_.size(); ++i) EXPECT_DOUBLE_EQ(0.0, immature_female.data_[i]) << " where i = " << i << "; age = " << i + immature_female.min_age_;
  for (unsigned i = 0; i < mature_male.data_.size(); ++i) EXPECT_DOUBLE_EQ(0.0, mature_male.data_[i]) << " where i = " << i << "; age = " << i + mature_male.min_age_;
  for (unsigned i = 0; i < mature_female.data_.size(); ++i) EXPECT_DOUBLE_EQ(0.0, mature_female.data_[i]) << " where i = " << i << "; age = " << i + mature_female.min_age_;

  /**
   * Do 1 iteration of the model then check the categories to see if
   * the maturation was successful
   */
  model_->FullIteration();

  // data * (1.0 - exp(-selectivities_[i]->GetAgeResult(category->min_age_ + j, category->age_length_) * ((1.0 - s) * ratio)));
  double immature_male_value   = 60000;
  double immature_female_value = 40000;

  auto logistic = model_->managers()->selectivity()->get("logistic");

  for (unsigned i = 0; i < immature_male.data_.size() && i < 15; ++i) {
    immature_male_value -= immature_male_value * (1.0 - exp(-logistic->GetAgeResult(1 + i, nullptr) * ((1.0 - 0.935) * 1)));
    immature_female_value -= immature_female_value * (1.0 - exp(-logistic->GetAgeResult(1 + i, nullptr) * ((1.0 - 0.935) * 1)));

    EXPECT_DOUBLE_EQ(immature_male_value, immature_male.data_[i]) << " where i = " << i << "; age = " << i + immature_male.min_age_;
    EXPECT_DOUBLE_EQ(immature_female_value, immature_female.data_[i]) << " where i = " << i << "; age = " << i + immature_female.min_age_;
  }

  // Mature should still be 0 because we had no maturation
  for (unsigned i = 0; i < mature_male.data_.size(); ++i) EXPECT_DOUBLE_EQ(0.0, mature_male.data_[i]) << " where i = " << i << "; age = " << i + mature_male.min_age_;
  for (unsigned i = 0; i < mature_female.data_.size(); ++i) EXPECT_DOUBLE_EQ(0.0, mature_female.data_[i]) << " where i = " << i << "; age = " << i + mature_female.min_age_;
}

/**
 * The test above uses a single s, a single time step and time_step_proportions of 1.0, so it
 * cannot detect a mix-up in per-category rate lookup, the selectivity index, or time step
 * proportions other than 1.0. The base below uses two categories with different s and
 * selectivities, and splits survival across two time steps.
 */
const std::string test_cases_process_survival_two_categories =
    R"(
@model
start_year 2000
final_year 2002
min_age 1
max_age 5
base_weight_units kgs
initialisation_phases iphase1
time_steps step_one step_two

@categories
format sex
names male female
age_lengths no_age_length no_age_length

@age_length no_age_length
type none
length_weight [type=none]

@initialisation_phase iphase1
years 2

@time_step step_one
processes ageing recruitment survival

@time_step step_two
processes survival

@ageing ageing
categories *

@Recruitment recruitment
type constant
categories male female
proportions 0.5 0.5
R0 100000
age 1
)";

/*
 * Two categories, differing s, logistic vs constant selectivity, split time steps
 */
TEST_F(InternalEmptyModel, Processes_Mortality_Survival_Rate_TwoTimeSteps_PerCategoryS) {
  const string survival = R"(
    @process survival
    type survival_constant_rate
    categories male female
    s 0.8 0.9
    time_step_proportions 0.4 0.6
    selectivities [type=logistic; a50=2; ato95=1] [type=constant; c=0.5]
  )";

  AddConfigurationLine(test_cases_process_survival_two_categories, __FILE__, 128);
  AddConfigurationLine(survival, __FILE__, 171);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  vector<double> expected_male   = {49502.491687458401, 44791.706764826413, 37040.91103408589, 30343.292605534407, 24843.711101682555};
  vector<double> expected_female = {47561.471225035697, 45241.870901797978, 43035.398821252886, 40936.537653899089, 38940.039153570237};
  partition::Category& male   = model_->partition().category("male");
  partition::Category& female = model_->partition().category("female");
  for (unsigned i = 0; i < expected_male.size(); ++i) {
    EXPECT_DOUBLE_EQ(expected_male[i], male.data_[i]) << " age index " << i;
    EXPECT_DOUBLE_EQ(expected_female[i], female.data_[i]) << " age index " << i;
  }
}

/*
 * time_step_proportions is optional for survival_constant_rate; omitted, each assigned time
 * step applies the full rate (ratio 1.0)
 */
TEST_F(InternalEmptyModel, Processes_Mortality_Survival_Rate_NoTimeStepProportions) {
  const string survival = R"(
    @process survival
    type survival_constant_rate
    categories male female
    s 0.8 0.9
    selectivities [type=logistic; a50=2; ato95=1] [type=constant; c=0.5]
  )";

  AddConfigurationLine(test_cases_process_survival_two_categories, __FILE__, 128);
  AddConfigurationLine(survival, __FILE__, 201);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  vector<double> expected_male   = {49009.933665337776, 40125.939898123936, 27440.581804701327, 18414.308122901592, 12344.199626077312};
  vector<double> expected_female = {45241.870901797978, 40936.537653899097, 37040.911034085904, 33516.002301781977, 30326.532985631682};
  partition::Category& male   = model_->partition().category("male");
  partition::Category& female = model_->partition().category("female");
  for (unsigned i = 0; i < expected_male.size(); ++i) {
    EXPECT_DOUBLE_EQ(expected_male[i], male.data_[i]) << " age index " << i;
    EXPECT_DOUBLE_EQ(expected_female[i], female.data_[i]) << " age index " << i;
  }
}

/*
 * survival_constant_rate's time_step_proportions have an exclusive lower bound of 0.0
 * (mortality_constant_rate accepts 0.0)
 */
TEST_F(InternalEmptyModel, Processes_Mortality_Survival_Rate_ZeroTimeStepProportion_Errors) {
  const string survival = R"(
    @process survival
    type survival_constant_rate
    categories male female
    s 0.8 0.9
    time_step_proportions 0.0 1.0
    selectivities [type=logistic; a50=2; ato95=1] [type=constant; c=0.5]
  )";

  AddConfigurationLine(test_cases_process_survival_two_categories, __FILE__, 128);
  AddConfigurationLine(survival, __FILE__, 230);
  LoadConfiguration();
  EXPECT_THROW(model_->Start(RunMode::kBasic), std::string);
}


/*
 * survival_constant_rate accepts relative_m_by_age as an alias for selectivities (added when it
 * was folded into MortalityConstantRemovalRate, matching mortality_constant_rate); the result must
 * be identical to Processes_Mortality_Survival_Rate_TwoTimeSteps_PerCategoryS
 */
TEST_F(InternalEmptyModel, Processes_Mortality_Survival_Rate_RelativeMByAgeAlias) {
  const string survival = R"(
    @process survival
    type survival_constant_rate
    categories male female
    s 0.8 0.9
    time_step_proportions 0.4 0.6
    relative_m_by_age [type=logistic; a50=2; ato95=1] [type=constant; c=0.5]
  )";

  AddConfigurationLine(test_cases_process_survival_two_categories, __FILE__, 128);
  AddConfigurationLine(survival, __FILE__, 252);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  vector<double> expected_male   = {49502.491687458401, 44791.706764826413, 37040.91103408589, 30343.292605534407, 24843.711101682555};
  vector<double> expected_female = {47561.471225035697, 45241.870901797978, 43035.398821252886, 40936.537653899089, 38940.039153570237};
  partition::Category& male   = model_->partition().category("male");
  partition::Category& female = model_->partition().category("female");
  for (unsigned i = 0; i < expected_male.size(); ++i) {
    EXPECT_DOUBLE_EQ(expected_male[i], male.data_[i]) << " age index " << i;
    EXPECT_DOUBLE_EQ(expected_female[i], female.data_[i]) << " age index " << i;
  }
}

}  // namespace niwa::processes::age
#endif  // TESTMODE
