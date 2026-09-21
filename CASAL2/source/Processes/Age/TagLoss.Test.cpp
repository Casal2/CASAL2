/**
 * @file TagLoss.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2025/05/08
 * @section LICENSE
 *
 * Copyright Casal2 Project 2025 - https://github.com/Casal2/
 *
 */
#ifdef TESTMODE

#include "TagLoss.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TestResources/TestFixtures/InternalEmptyModel.h"

namespace niwa::processes::age {
using niwa::testfixtures::InternalEmptyModel;

const std::string test_cases_process_tag_loss =
    R"(
@model
start_year 2000
final_year 2002
min_age 1
max_age 5
base_weight_units kgs
initialisation_phases iphase1
time_steps step_one

@categories
format sex
names tagged
age_lengths no_age_length

@age_length no_age_length
type none
length_weight [type=none]

@initialisation_phase iphase1
years 2

@time_step step_one
processes ageing recruitment tag_loss

@ageing ageing
categories *

@Recruitment recruitment
type constant
categories tagged
proportions 1.0
R0 100000
age 1
)";

/*
 * Sample unit test
 */
TEST_F(InternalEmptyModel, Processes_TagLoss) {
  const string tag_loss = R"(
    @process tag_loss
    type tag_loss
    categories tagged
    time_step_proportions 1.0
    tag_loss_rate 0.25
    selectivities loss_rate1=[type=constant; c=1]
    tag_loss_type single
    year 2002
  )";

  AddConfigurationLine(test_cases_process_tag_loss, __FILE__, 24);
  AddConfigurationLine(tag_loss, __FILE__, 64);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  partition::Category& tagged = model_->partition().category("tagged");
  EXPECT_DOUBLE_EQ(77880.078307140488, tagged.data_[0]);
  EXPECT_DOUBLE_EQ(77880.078307140488, tagged.data_[1]);
  EXPECT_DOUBLE_EQ(77880.078307140488, tagged.data_[2]);
  EXPECT_DOUBLE_EQ(77880.078307140488, tagged.data_[3]);
  EXPECT_DOUBLE_EQ(77880.078307140488, tagged.data_[4]);
}

/*
 * Sample unit test
 */
TEST_F(InternalEmptyModel, Processes_TagLoss_MultipleYears) {
  const string tag_loss = R"(
    @process tag_loss
    type tag_loss
    categories tagged
    time_step_proportions 1.0
    tag_loss_rate 0.25
    selectivities loss_rate1=[type=constant; c=1]
    tag_loss_type single
    year 2000
  )";

  AddConfigurationLine(test_cases_process_tag_loss, __FILE__, 24);
  AddConfigurationLine(tag_loss, __FILE__, 64);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  partition::Category& tagged = model_->partition().category("tagged");
  EXPECT_DOUBLE_EQ(77880.078307140488, tagged.data_[0]);
  EXPECT_DOUBLE_EQ(60653.065971263342, tagged.data_[1]);
  EXPECT_DOUBLE_EQ(47236.655274101475, tagged.data_[2]);
  EXPECT_DOUBLE_EQ(36787.944117144238, tagged.data_[3]);
  EXPECT_DOUBLE_EQ(28650.479686019014, tagged.data_[4]);
}

/*
 * Sample unit test
 */
TEST_F(InternalEmptyModel, Processes_TagLoss_Double) {
  const string tag_loss = R"(
    @process tag_loss
    type tag_loss
    categories tagged
    time_step_proportions 1.0
    tag_loss_rate 0.25
    selectivities loss_rate1=[type=constant; c=1]
    tag_loss_type double
    year 2002
  )";

  AddConfigurationLine(test_cases_process_tag_loss, __FILE__, 24);
  AddConfigurationLine(tag_loss, __FILE__, 64);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  partition::Category& tagged = model_->partition().category("tagged");
  EXPECT_DOUBLE_EQ(100000, tagged.data_[0]);
  EXPECT_DOUBLE_EQ(100000, tagged.data_[1]);
  EXPECT_DOUBLE_EQ(100000, tagged.data_[2]);
  EXPECT_DOUBLE_EQ(100000, tagged.data_[3]);
  EXPECT_DOUBLE_EQ(100000, tagged.data_[4]);
}

/*
 * Sample unit test
 */
TEST_F(InternalEmptyModel, Processes_TagLoss_Double_MultipleYears) {
  const string tag_loss = R"(
    @process tag_loss
    type tag_loss
    categories tagged
    time_step_proportions 1.0
    tag_loss_rate 0.25
    selectivities loss_rate1=[type=constant; c=1]
    tag_loss_type double
    year 2000
  )";

  AddConfigurationLine(test_cases_process_tag_loss, __FILE__, 24);
  AddConfigurationLine(tag_loss, __FILE__, 64);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  partition::Category& tagged = model_->partition().category("tagged");
  EXPECT_DOUBLE_EQ(88866.337151053871, tagged.data_[0]);
  EXPECT_DOUBLE_EQ(84518.187825382454, tagged.data_[1]);
  EXPECT_DOUBLE_EQ(84518.187825382454, tagged.data_[2]);
  EXPECT_DOUBLE_EQ(84518.187825382454, tagged.data_[3]);
  EXPECT_DOUBLE_EQ(84518.187825382454, tagged.data_[4]);
}

/**
 * The tests above all use a single category, a constant c=1 selectivity and a single time step,
 * so they cannot detect a mix-up in per-category rate lookup, the selectivity index, age-varying
 * selectivity, or time step proportions other than 1.0. The base below uses two categories with
 * different rates and selectivities, and splits tag loss across two time steps.
 */
const std::string test_cases_process_tag_loss_two_categories =
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
names tagged_a tagged_b
age_lengths no_age_length no_age_length

@age_length no_age_length
type none
length_weight [type=none]

@initialisation_phase iphase1
years 2

@time_step step_one
processes ageing recruitment tag_loss

@time_step step_two
processes tag_loss

@ageing ageing
categories *

@Recruitment recruitment
type constant
categories tagged_a tagged_b
proportions 0.5 0.5
R0 100000
age 1
)";

/*
 * Two categories, differing rates, logistic vs constant selectivity, split time steps
 */
TEST_F(InternalEmptyModel, Processes_TagLoss_TwoCategories_WeightedSelectivity) {
  const string tag_loss = R"(
    @process tag_loss
    type tag_loss
    categories tagged_a tagged_b
    time_step_proportions 0.4 0.6
    tag_loss_rate 0.1 0.3
    selectivities [type=logistic; a50=2; ato95=1] [type=constant; c=0.5]
    tag_loss_type single
    year 2000
  )";

  AddConfigurationLine(test_cases_process_tag_loss_two_categories, __FILE__, 184);
  AddConfigurationLine(tag_loss, __FILE__, 227);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  vector<double> expected_a = {49750.623959634118, 47324.257397674191, 43035.398821252893, 38950.797556362319, 35244.652858045403};
  vector<double> expected_b = {43035.398821252893, 37040.911034085897, 31881.407581088664, 27440.581804701324, 23618.327637050737};
  partition::Category& tagged_a = model_->partition().category("tagged_a");
  partition::Category& tagged_b = model_->partition().category("tagged_b");
  for (unsigned i = 0; i < expected_a.size(); ++i) {
    EXPECT_DOUBLE_EQ(expected_a[i], tagged_a.data_[i]) << " age index " << i;
    EXPECT_DOUBLE_EQ(expected_b[i], tagged_b.data_[i]) << " age index " << i;
  }
}

/*
 * As above, with double tag loss, which derives a rate from years since tagging
 */
TEST_F(InternalEmptyModel, Processes_TagLoss_Double_TwoCategories_WeightedSelectivity) {
  const string tag_loss = R"(
    @process tag_loss
    type tag_loss
    categories tagged_a tagged_b
    time_step_proportions 0.4 0.6
    tag_loss_rate 0.1 0.3
    selectivities [type=logistic; a50=2; ato95=1] [type=constant; c=0.5]
    tag_loss_type double
    year 2000
  )";

  AddConfigurationLine(test_cases_process_tag_loss_two_categories, __FILE__, 184);
  AddConfigurationLine(tag_loss, __FILE__, 256);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  vector<double> expected_a = {49939.253592319204, 49373.383285634154, 48636.620292499712, 48382.323074397667, 48358.459641987909};
  vector<double> expected_b = {46200.163276641491, 44621.437120397939, 44621.437120397939, 44621.437120397939, 44621.437120397939};
  partition::Category& tagged_a = model_->partition().category("tagged_a");
  partition::Category& tagged_b = model_->partition().category("tagged_b");
  for (unsigned i = 0; i < expected_a.size(); ++i) {
    EXPECT_DOUBLE_EQ(expected_a[i], tagged_a.data_[i]) << " age index " << i;
    EXPECT_DOUBLE_EQ(expected_b[i], tagged_b.data_[i]) << " age index " << i;
  }
}

/*
 * An unrecognised tag_loss_type used to silently apply no tag loss; it is now an error
 */
TEST_F(InternalEmptyModel, Processes_TagLoss_InvalidTagLossType) {
  const string tag_loss = R"(
    @process tag_loss
    type tag_loss
    categories tagged
    time_step_proportions 1.0
    tag_loss_rate 0.25
    selectivities loss_rate1=[type=constant; c=1]
    tag_loss_type triple
    year 2000
  )";

  AddConfigurationLine(test_cases_process_tag_loss, __FILE__, 24);
  AddConfigurationLine(tag_loss, __FILE__, 64);
  LoadConfiguration();
  EXPECT_THROW(model_->Start(RunMode::kBasic), std::string);
}

}  // namespace niwa::processes::age
#endif  // TESTMODE