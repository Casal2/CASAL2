/**
 * @file ProcessRemovalsByAgeRetained.Test.cpp
 * @author Samik Datta (samik.datta@niwa.co.nz)
 * @github https://github.com/SamikDatta
 * @date 07/05/2019
 * @section LICENSE
 *
 * Copyright NIWA Science ©2019 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "ProcessRemovalsByAgeRetained.h"

#include <iostream>

#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Observations/Manager.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"

// Namespaces
namespace niwa {
namespace age {

using std::cout;
using std::endl;
using niwa::testfixtures::InternalEmptyModel;

const std::string test_cases_observation_process_removals_by_age_retained =
R"(
@model
start_year 2001
final_year 2005
min_age 1
max_age 20
age_plus true
base_weight_units tonnes
initialisation_phases Define_manually
time_steps 1

@categories
format sex
names   male
age_lengths asMm0

@initialisation_phase Define_manually
type state_category_by_age
categories male
min_age 1
max_age 10
table n
male 100000 90000 80000 70000 60000 50000 40000 30000 20000 10000
end_table

@time_step 1
processes mort

@process mort
type mortality_instantaneous_retained
m 0
time_step_ratio 1
relative_m_by_age One
categories *
table catches
year FishingPot
2005  8015.123
end_table
table method
method     category    selectivity         retained_selectivity discard_mortality_selectivity u_max time_step penalty
FishingPot format=male potFSel_length_male potRet_male          fifty_percent                 0.7   1         CatchMustBeTaken1
end_table

@selectivity One
type constant
c 1

@selectivity fifty_percent
type constant
c 0.5

@selectivity potFSel_length_male
type logistic
a50   4
ato95  1
length_based False

@selectivity potRet_male
type logistic
a50   6
ato95  1
length_based False

@penalty CatchMustBeTaken1
type process
log_scale True
multiplier 1000

@age_length asMm0
type von_bertalanffy
by_length true
linf 55.7
k 0.14
t0 -0.82
cv_first 0.1
cv_last 0.1
length_weight Length_Weight

@length_weight Length_Weight
type basic
units kgs
a 0.007289
b 3.2055

@report population_sizes
type partition
time_step 1
years 2001:2005

@observation potFishAFretained
type process_removals_by_age_retained
mortality_instantaneous_process mort
method_of_removal FishingPot
years 2005
time_step 1
categories male
ageing_error no_error
min_age 3
max_age 15
plus_group True
table obs
2005 1.650990e-10 7.566419e-07 1.771126e-03 1.962050e-01 3.192775e-01 2.413644e-01 1.609208e-01 8.046047e-02 0 0 0 0 0
end_table
table error_values
2005 651
end_table
likelihood multinomial
delta 1e-11

@ageing_error no_error
type none

)";

/**
 *
 */
TEST_F(InternalEmptyModel, Observation_Process_Removals_By_Age_Retained) {
  AddConfigurationLine(test_cases_observation_process_removals_by_age_retained, __FILE__, 31);
  LoadConfiguration();

  model_->Start(RunMode::kBasic); // kEstimation instead of kBasic

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_NEAR(24.9387, obj_function.score(), 1e-4);


  Observation* observation = model_->managers().observation()->GetObservation("potFishAFretained");

  map<unsigned, vector<obs::Comparison> >& comparisons = observation->comparisons();
  ASSERT_EQ(1u, comparisons.size());

  unsigned year = 2005;
  ASSERT_FALSE(comparisons.find(year) == comparisons.end());
  ASSERT_EQ(13u, comparisons[year].size());

  // age 3
  EXPECT_EQ("male", comparisons[year][0].category_);
  EXPECT_EQ(3, comparisons[year][0].age_);
  EXPECT_NEAR(1.650990e-10, comparisons[year][0].observed_, 1e-6);
  EXPECT_NEAR(4.63555e-006, comparisons[year][0].expected_, 1e-6);
  EXPECT_NEAR(1.258e-006, comparisons[year][0].score_, 1e-6);

  // age 6
  EXPECT_EQ("male", comparisons[year][3].category_);
  EXPECT_EQ(6, comparisons[year][3].age_);
  EXPECT_NEAR(1.962050e-01, comparisons[year][3].observed_, 1e-6);
  EXPECT_NEAR(0.1982, comparisons[year][3].expected_, 1e-4);
  EXPECT_NEAR(701.819, comparisons[year][3].score_, 1e-3);

  // age 9
  EXPECT_EQ("male", comparisons[year][6].category_);
  EXPECT_EQ(9, comparisons[year][6].age_);
  EXPECT_NEAR(1.609208e-01, comparisons[year][6].observed_, 1e-6);
  EXPECT_NEAR(0.158976, comparisons[year][6].expected_, 1e-6);
  EXPECT_NEAR(578.445, comparisons[year][6].score_, 1e-3);

}

} /* namespace processes */
} /* namespace niwa */


#endif /* TESTMODE */
