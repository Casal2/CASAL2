/**
 * @file RecruitmentBevertonHolt.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 20/08/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include <iostream>

#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Processes/Manager.h"
#include "Model/Model.h"
#include "Partition/Partition.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"
#include "TestResources/Models/CasalComplex1.h"

// Namespaces
namespace niwa {
namespace processes {

using std::cout;
using std::endl;
using niwa::testfixtures::InternalEmptyModel;

/**
 *
 */
TEST_F(InternalEmptyModel, Processes_BevertonHolt_Recruitment) {
  AddConfigurationLine(testresources::models::test_cases_models_casal_complex_1, "TestResources/Models/CasalComplex1.h", 32);
  LoadConfiguration();

  ModelPtr model = Model::Instance();
  model->Start(RunMode::kBasic);

  partition::Category& male   = Partition::Instance().category("male");
  EXPECT_DOUBLE_EQ(0.0, male.data_[0]);
  EXPECT_DOUBLE_EQ(2499927.5474240803, male.data_[1]);
  EXPECT_DOUBLE_EQ(2046756.2758644461, male.data_[2]);

  EXPECT_DOUBLE_EQ(151391.67827268399, male.data_[15]);
  EXPECT_DOUBLE_EQ(123889.51433539754, male.data_[16]);
  EXPECT_DOUBLE_EQ(101388.98722028485, male.data_[17]);

  EXPECT_DOUBLE_EQ(45509.118554179389, male.data_[21]);
  EXPECT_DOUBLE_EQ(37254.617197639338, male.data_[22]);
  EXPECT_DOUBLE_EQ(168174.98846106316, male.data_[23]);

  partition::Category& female = Partition::Instance().category("female");
  EXPECT_DOUBLE_EQ(0.0, male.data_[0]);
  EXPECT_DOUBLE_EQ(2499927.5474240803, female.data_[1]);
  EXPECT_DOUBLE_EQ(2046760.4571484025, female.data_[2]);

  EXPECT_DOUBLE_EQ(151581.19489755956, female.data_[15]);
  EXPECT_DOUBLE_EQ(124062.47306900102, female.data_[16]);
  EXPECT_DOUBLE_EQ(101543.48455296629, female.data_[17]);

  EXPECT_DOUBLE_EQ(45592.839274420774, female.data_[21]);
  EXPECT_DOUBLE_EQ(37324.680701432851, female.data_[22]);
  EXPECT_DOUBLE_EQ(168519.01800652978, female.data_[23]);
}

/**
 *
 */
const std::string test_cases_process_recruitment_bh =
R"(
@model
start_year 1994
final_year 2008
min_age 1
max_age 12
age_plus t
initialisation_phases iphase1 iphase2
time_steps step_one=[processes=Recruitment] step_two=[processes=Tagging] step_three=[processes=Ageing]

@categories
format stage.sex
names immature.male mature.male immature.female mature.female

@initialisation_phase iphase1
years 200

@initialisation_phase iphase2
years 1

@ageing Ageing
categories *

@Recruitment Recruitment
type constant
categories immature.male immature.female
proportions 0.5 0.5
R0 997386
age 1

@tag Tagging
type by_age
years 2008
from immature.male immature.female
to mature.male mature.female
selectivities [type=constant; c=0.25] [type=constant; c=0.4]
min_age 3
max_age 6
loss_rate 0
loss_rate_selectivities [type=constant; c=1] [type=constant; c=1]
table numbers
year 3 4 5 6
2008 1000 2000 3000 4000
end_table
)";

} /* namespace processes */
} /* namespace niwa */


#endif /* TESTMODE */
