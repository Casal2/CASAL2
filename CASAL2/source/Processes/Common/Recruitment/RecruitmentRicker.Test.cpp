/**
 * @file RecruitmentRicker.Test.cpp
 * @author  A Dunn
 * @date June 2023
 * @section LICENSE
 *
 * Copyright Casal2 Project 2025 - https://github.com/Casal2/
 *
 */
#ifdef TESTMODE

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>

#include "DerivedQuantities/Manager.h"
#include "Model/Factory.h"
#include "Model/Managers.h"
#include "Model/Model.h"
#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Partition/Partition.h"
#include "Selectivities/Manager.h"
#include "Selectivities/Selectivity.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"
#include "TimeSteps/Manager.h"

namespace niwa::processes::common {
using niwa::testfixtures::InternalEmptyModel;
using std::cout;
using std::endl;

const string model_definition =
    R"(
@model
start_year 1994
final_year 2012
projection_final_year 2016
min_age 1
max_age 12
base_weight_units tonnes
age_plus true
initialisation_phases iphase1 iphase2
time_steps step_one step_two

@categories
format stage.sex
names immature,mature.male,female
age_lengths age_length_male*2 age_length_female*2

@age_length age_length_male
type von_bertalanffy
k 0.261
t0 -0.50
linf 92.6
time_step_proportions 0.4 0.6
length_weight length_weight_basic

@age_length age_length_female
type von_bertalanffy
k 0.213
t0 -0.6
linf 104
time_step_proportions 0.25 0.75
length_weight length_weight_basic

@length_weight length_weight_basic
type basic
units tonnes
a 4.79e-09
b 2.89

@initialisation_phase iphase1
years 200

@initialisation_phase iphase2
years 1

@time_step step_one
processes Recruitment

@time_step step_two
processes Ageing

@process Recruitment
type recruitment_ricker
categories immature.male immature.female
proportions 0.6 0.4
b0 5973.9
steepness 0.5792984
ssb SSB
age 1
recruitment_multipliers 0.0935823574532 0.454148603651 0.143755692424 0.979662527968 1.70885246646 0.207878584523 0.142998000652 0.448480496158 1.25142717617 0.621050539438 0.0732265269830 0.0247523483886 0.137793954591 0.199439856167 0.701101119095 0.613584736138 0.103102553946 0.101487357908 0.412014325737
standardise_years 1994 1995 1996 1997

@process Ageing
type ageing
categories *

@derived_quantity SSB
type biomass
time_step step_one
categories *
time_step_proportion 0.35
time_step_proportion_method weighted_sum
selectivities Logistic*4

@selectivity Logistic
type logistic
a50 8
ato95 3
)";

TEST_F(InternalEmptyModel, Processes_Recruitment_Ricker) {
  AddConfigurationLine(model_definition, __FILE__, 33);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(0.0, obj_function.score());

  partition::Category& immature_male   = model_->partition().category("immature.male");
  partition::Category& immature_female = model_->partition().category("immature.female");

  // Expected values for comparison - these are tailored for the Ricker recruitment
  // These values will need to be updated after running the test once to see actual values
  vector<double> expected_immature_male   = {0.0,
                                             3625.696621418153882,
                                             893.950198960341254,
                                             909.225899948130063,
                                             5417.840702133305967,
                                             6199.399360299527871,
                                             1766.412818682717443,
                                             1223.021972115345534,
                                             220.210417367492397,
                                             652.884216372045330,
                                             5547.465200982328497,
                                             1909716.028394282096997};
  vector<double> expected_immature_female = {0.0,
                                             2417.131080945436224,
                                             595.966799306894131,
                                             606.150599965420042,
                                             3611.893801422203978,
                                             4132.932906866352823,
                                             1177.608545788478295,
                                             815.347981410230432,
                                             146.806944911661589,
                                             435.256144248030296,
                                             3698.310133988219150,
                                             1273144.018929526209831};

  // Validate the values in the partition::Category objects
  for (size_t i = 0; i < expected_immature_male.size(); ++i) {
    EXPECT_NEAR(expected_immature_male[i], immature_male.data_[i], 1e-10) << "Mismatch in immature.male at index " << i;
  }

  for (size_t i = 0; i < expected_immature_female.size(); ++i) {
    EXPECT_NEAR(expected_immature_female[i], immature_female.data_[i], 1e-10) << "Mismatch in immature.female at index " << i;
  }
}

/**
 * Simplex auto-detection test.
 * Identical model to Processes_Recruitment_Ricker except:
 *   - no explicit standardise_years
 *   - @parameter_transformation simplex on recruitment_multipliers{1994:1997}
 * The simplex covers the same 4 years as the explicit standardise_years in the base test.
 * auto-detected standardise_years = {1994, 1995, 1996, 1997} => identical mean_ycs_
 * => identical standardised values => identical partition output.
 */
const string model_definition_simplex =
    R"(
@model
start_year 1994
final_year 2012
projection_final_year 2016
min_age 1
max_age 12
base_weight_units tonnes
age_plus true
initialisation_phases iphase1 iphase2
time_steps step_one step_two

@categories
format stage.sex
names immature,mature.male,female
age_lengths age_length_male*2 age_length_female*2

@age_length age_length_male
type von_bertalanffy
k 0.261
t0 -0.50
linf 92.6
time_step_proportions 0.4 0.6
length_weight length_weight_basic

@age_length age_length_female
type von_bertalanffy
k 0.213
t0 -0.6
linf 104
time_step_proportions 0.25 0.75
length_weight length_weight_basic

@length_weight length_weight_basic
type basic
units tonnes
a 4.79e-09
b 2.89

@initialisation_phase iphase1
years 200

@initialisation_phase iphase2
years 1

@time_step step_one
processes Recruitment

@time_step step_two
processes Ageing

@process Recruitment
type recruitment_ricker
categories immature.male immature.female
proportions 0.6 0.4
b0 5973.9
steepness 0.5792984
ssb SSB
age 1
recruitment_multipliers 0.0935823574532 0.454148603651 0.143755692424 0.979662527968 1.70885246646 0.207878584523 0.142998000652 0.448480496158 1.25142717617 0.621050539438 0.0732265269830 0.0247523483886 0.137793954591 0.199439856167 0.701101119095 0.613584736138 0.103102553946 0.101487357908 0.412014325737

@process Ageing
type ageing
categories *

@derived_quantity SSB
type biomass
time_step step_one
categories *
time_step_proportion 0.35
time_step_proportion_method weighted_sum
selectivities Logistic*4

@selectivity Logistic
type logistic
a50 8
ato95 3

@parameter_transformation simplex_ycs
type simplex
parameters process[Recruitment].recruitment_multipliers{1994:1997}
sum_to_one false

@estimate ycs_simp
type uniform
parameter parameter_transformation[simplex_ycs].simplex
lower_bound -10*3
upper_bound 10*3
)";

TEST_F(InternalEmptyModel, Processes_Recruitment_Ricker_Simplex_AutoDetectsStandardiseYears) {
  AddConfigurationLine(model_definition_simplex, __FILE__, __LINE__);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  // With simplex on recruitment_multipliers{1994:1997}, auto-detect sets
  // standardise_years = {1994, 1995, 1996, 1997} -- the same 4 years as
  // the explicit specification in Processes_Recruitment_Ricker.
  // mean_ycs_ and standardised values are identical, so partition output matches.
  partition::Category& immature_male   = model_->partition().category("immature.male");
  partition::Category& immature_female = model_->partition().category("immature.female");

  vector<double> expected_immature_male = {0.0,
                                           3625.696621418153882,
                                           893.950198960341254,
                                           909.225899948130063,
                                           5417.840702133305967,
                                           6199.399360299527871,
                                           1766.412818682717443,
                                           1223.021972115345534,
                                           220.210417367492397,
                                           652.884216372045330,
                                           5547.465200982328497,
                                           1909716.028394282096997};

  vector<double> expected_immature_female = {0.0,
                                             2417.131080945436224,
                                             595.966799306894131,
                                             606.150599965420042,
                                             3611.893801422203978,
                                             4132.932906866352823,
                                             1177.608545788478295,
                                             815.347981410230432,
                                             146.806944911661589,
                                             435.256144248030296,
                                             3698.310133988219150,
                                             1273144.018929526209831};

  for (size_t i = 0; i < expected_immature_male.size(); ++i) EXPECT_NEAR(expected_immature_male[i], immature_male.data_[i], 1e-10) << "Mismatch in immature.male at index " << i;

  for (size_t i = 0; i < expected_immature_female.size(); ++i)
    EXPECT_NEAR(expected_immature_female[i], immature_female.data_[i], 1e-10) << "Mismatch in immature.female at index " << i;
}

}  // namespace niwa::processes::common
#endif  // TESTMODE
