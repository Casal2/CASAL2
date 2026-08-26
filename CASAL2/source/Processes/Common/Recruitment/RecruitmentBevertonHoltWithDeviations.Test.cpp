/**
 * @file RecruitmentBevertonHoltWithDeviations.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2025/03/25
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
type recruitment_beverton_holt_with_deviations
categories immature.male immature.female
proportions 0.6 0.4
b0 5973.9
last_year_with_no_bias 1994
first_year_with_bias 1999
last_year_with_bias 2006
first_recent_year_with_no_bias 2008
b_max 1.0
deviation_values 0.0935823574532 -0.454148603651 -0.143755692424 -0.979662527968 1.70885246646 0.207878584523 -0.142998000652 0.448480496158 -1.25142717617 0.621050539438 0.0732265269830 0.0247523483886 0.137793954591 0.199439856167 -0.701101119095 0.613584736138 -0.103102553946 -0.101487357908 -0.412014325737
steepness 0.5792984
ssb SSB
age 1
sigma_r 0.6

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

TEST_F(InternalEmptyModel, Processes_Recruitment_Beverton_Holt_With_Deviations) {
  AddConfigurationLine(model_definition, __FILE__, 33);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(0.0, obj_function.score());

  partition::Category& immature_male   = model_->partition().category("immature.male");
  partition::Category& immature_female = model_->partition().category("immature.female");

  // Expected values for comparison
  std::vector<double> expected_immature_male   = {0.0, 6146.990226562394, 8379.130986959304, 8359.405234144671, 17101.423424200115, 4587.998027370768,
                                                  0.0, 8844.086809047682, 7888.866777306661, 8272.523547339177, 14295.511530930216, 1936654.687011178};
  std::vector<double> expected_immature_female = {0.0, 4097.99348437493,  5586.087324639537, 5572.936822763115, 11400.948949466745, 3058.665351580512,
                                                  0.0, 5896.057872698455, 5259.244518204441, 5515.015698226118, 9530.341020620144,  1291103.1246741235};

  // Validate the values in the partition::Category objects
  for (size_t i = 0; i < expected_immature_male.size(); ++i) {
    EXPECT_NEAR(expected_immature_male[i], immature_male.data_[i], 1e-9) << "Mismatch in immature.male at index " << i;
  }

  for (size_t i = 0; i < expected_immature_female.size(); ++i) {
    EXPECT_NEAR(expected_immature_female[i], immature_female.data_[i], 1e-9) << "Mismatch in immature.female at index " << i;
  }
}

}  // namespace niwa::processes::common
#endif  // TESTMODE
