/**
 * @file TagLossEmpirical.Test.cpp
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
TEST_F(InternalEmptyModel, Processes_TagLossEmpirical) {
  const string tag_loss = R"(
    @process tag_loss
    type tag_loss_empirical
    categories tagged
    time_step_proportions 1.0
    tag_loss_rate 0.25
    selectivities loss_rate1=[type=constant; c=1]
    year 2002
    years_at_liberty 0
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
TEST_F(InternalEmptyModel, Processes_TagLossEmpirical_MultipleYears) {
  const string tag_loss = R"(
    @process tag_loss
    type tag_loss_empirical
    categories tagged
    time_step_proportions 1.0
    tag_loss_rate 0.25
    selectivities loss_rate1=[type=constant; c=1]
    year 2000
    years_at_liberty 1 2
  )";

  AddConfigurationLine(test_cases_process_tag_loss, __FILE__, 24);
  AddConfigurationLine(tag_loss, __FILE__, 64);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  partition::Category& tagged = model_->partition().category("tagged");
  EXPECT_DOUBLE_EQ(77880.078307140488, tagged.data_[0]);
  EXPECT_DOUBLE_EQ(60653.065971263342, tagged.data_[1]);
  EXPECT_DOUBLE_EQ(60653.065971263342, tagged.data_[2]);
  EXPECT_DOUBLE_EQ(60653.065971263342, tagged.data_[3]);
  EXPECT_DOUBLE_EQ(60653.065971263342, tagged.data_[4]);
}

/**
 * The tests above all use a single category, a constant c=1 selectivity, a single time step and
 * the same rate in every year at liberty. The base below uses two categories with different
 * selectivities, splits tag loss across two time steps, and supplies a different rate for each
 * year at liberty.
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
 * Two categories, logistic vs constant selectivity, split time steps, a rate per year at liberty
 */
TEST_F(InternalEmptyModel, Processes_TagLossEmpirical_TwoCategories_WeightedSelectivity) {
  const string tag_loss = R"(
    @process tag_loss
    type tag_loss_empirical
    categories tagged_a tagged_b
    time_step_proportions 0.4 0.6
    tag_loss_rate 0.1 0.3
    selectivities [type=logistic; a50=2; ato95=1] [type=constant; c=0.5]
    year 2000
    years_at_liberty 1 2
  )";

  AddConfigurationLine(test_cases_process_tag_loss_two_categories, __FILE__, 126);
  AddConfigurationLine(tag_loss, __FILE__, 169);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  vector<double> expected_a = {49255.596980153132, 42820.758874180676, 35766.904317627988, 33711.928451348031, 33526.728296272944};
  vector<double> expected_b = {43035.398821252893, 40936.537653899097, 40936.537653899097, 40936.537653899097, 40936.537653899097};
  partition::Category& tagged_a = model_->partition().category("tagged_a");
  partition::Category& tagged_b = model_->partition().category("tagged_b");
  for (unsigned i = 0; i < expected_a.size(); ++i) {
    EXPECT_DOUBLE_EQ(expected_a[i], tagged_a.data_[i]) << " age index " << i;
    EXPECT_DOUBLE_EQ(expected_b[i], tagged_b.data_[i]) << " age index " << i;
  }
}

/*
 * A repeated year at liberty used to index past the end of the selectivities; it is now an error
 */
TEST_F(InternalEmptyModel, Processes_TagLossEmpirical_DuplicateYearsAtLiberty) {
  const string tag_loss = R"(
    @process tag_loss
    type tag_loss_empirical
    categories tagged
    time_step_proportions 1.0
    tag_loss_rate 0.25
    selectivities loss_rate1=[type=constant; c=1]
    year 2000
    years_at_liberty 1 1
  )";

  AddConfigurationLine(test_cases_process_tag_loss, __FILE__, 24);
  AddConfigurationLine(tag_loss, __FILE__, 64);
  LoadConfiguration();
  EXPECT_THROW(model_->Start(RunMode::kBasic), std::string);
}

}  // namespace niwa::processes::age
#endif  // TESTMODE