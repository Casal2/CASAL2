/**
 * @file TransitionCategoryByAge.Test.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 9/01/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Test cases for the @transition.category_by_age process
 */
#ifdef TESTMODE

// Headers
#include <iostream>

#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Processes/Manager.h"
#include "Model/Model.h"
#include "Partition/Partition.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"

// Namespaces
namespace niwa {
namespace processes {
namespace age {


using std::cout;
using std::endl;
using niwa::testfixtures::InternalEmptyModel;


const std::string test_cases_process_transition_category_by_age =
R"(
@model
start_year 1994
final_year 2008
min_age 1
max_age 50
age_plus t
initialisation_phases iphase1 iphase2
time_steps step_one step_one_pt_five step_two

@categories
format stage.sex
names immature.male mature.male immature.female mature.female

@initialisation_phase iphase1
years 200

@initialisation_phase iphase2
years 1

@time_step step_one
processes Recruitment halfM halfM

@time_step step_one_pt_five
processes transition

@time_step step_two
processes my_ageing

@ageing my_ageing
categories immature.male mature.male immature.female mature.female

@Recruitment Recruitment
type constant
categories immature.male immature.female
proportions 0.5 0.5
R0 997386
age 1

@mortality halfM
type constant_rate
categories immature.male mature.male immature.female mature.female
M 0.065 0.065 0.045 0.045
selectivities One One One One

@transition transition
type category_by_age
years 2008
from immature.male immature.female
to mature.male mature.female
min_age 3
max_age 6
table n
year 3 4 5 6
2008 1000 2000 3000 4000
end_table

@selectivity One
type constant
c 1

@selectivity Maturation
type logistic_producing
L 5
H 30
a50 8
ato95 3

@selectivity FishingSel
type logistic
a50 8
ato95 3

@report DQ
type derived_quantity
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Processes_Transition_Category_By_Age) {
  AddConfigurationLine(test_cases_process_transition_category_by_age, __FILE__, 37);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  partition::Category& male   = model_->partition().category("mature.male");
  EXPECT_DOUBLE_EQ(0.0,         male.data_[0]);
  EXPECT_DOUBLE_EQ(0.0,         male.data_[1]);
  EXPECT_DOUBLE_EQ(0.0,         male.data_[2]);
  EXPECT_DOUBLE_EQ(470.03594823542818,  male.data_[3]);
  EXPECT_DOUBLE_EQ(920.17023088886867,  male.data_[4]);
  EXPECT_DOUBLE_EQ(1350.4980080625662,  male.data_[5]);
  EXPECT_DOUBLE_EQ(1761.1454029312279,  male.data_[6]);
  EXPECT_DOUBLE_EQ(0.0,                 male.data_[7]);

  partition::Category& female = model_->partition().category("mature.female");
  EXPECT_DOUBLE_EQ(0.0,         female.data_[0]);
  EXPECT_DOUBLE_EQ(0.0,         female.data_[1]);
  EXPECT_DOUBLE_EQ(0.0,         female.data_[2]);
  EXPECT_DOUBLE_EQ(529.96405176457176,  female.data_[3]);
  EXPECT_DOUBLE_EQ(1079.8297691111316,  female.data_[4]);
  EXPECT_DOUBLE_EQ(1649.501991937434,   female.data_[5]);
  EXPECT_DOUBLE_EQ(2238.8545970687719,  female.data_[6]);
  EXPECT_DOUBLE_EQ(0.0,                 female.data_[7]);
}

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
#endif /* TESTMODE */
