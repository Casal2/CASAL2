/**
 * @file ProcessRemovalsByLengthRetained.Test.cpp
 * @author Samik Datta (samik.datta@niwa.co.nz)
 * @github https://github.com/SamikDatta
 * @date 07/05/2019
 * @section LICENSE
 *
 * Copyright NIWA Science �2019 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "ProcessRemovalsByLengthRetained.h"

#include <iostream>

#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Observations/Manager.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"

// Namespaces
namespace niwa {
namespace age {

using niwa::testfixtures::InternalEmptyModel;
using std::cout;
using std::endl;

const std::string test_cases_observation_process_removals_by_length_retained =
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
length_bins 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46
length_plus False

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
time_step_proportions 1
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

@observation potFishLFretained
type process_removals_by_length_retained
mortality_instantaneous_process mort
method_of_removal FishingPot
years 2005
time_step 1
categories male
length_bins 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46
length_plus False
table obs
2005 0.02462879 0.03536036 0.04759163 0.06025858 0.07205340 0.08169356 0.08817806 0.09095124 0.08994426 0.08551446 0.07832640 0.06921554 0.05906081 0.04868002 0.03875492 0.02978796
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
TEST_F(InternalEmptyModel, Observation_Process_Removals_By_Length_Retained) {
  AddConfigurationLine(test_cases_observation_process_removals_by_length_retained, __FILE__, 31);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);  // kEstimation instead of kBasic

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_NEAR(39.887, obj_function.score(), 1e-3);

  Observation* observation = model_->managers()->observation()->GetObservation("potFishLFretained");

  map<unsigned, vector<obs::Comparison> >& comparisons = observation->comparisons();
  ASSERT_EQ(1u, comparisons.size());

  unsigned year = 2005;
  ASSERT_FALSE(comparisons.find(year) == comparisons.end());
  ASSERT_EQ(16u, comparisons[year].size());

  // age 3
  EXPECT_EQ("male", comparisons[year][0].category_);
  EXPECT_EQ(30, comparisons[year][0].length_);
  EXPECT_NEAR(0.02462879, comparisons[year][0].observed_, 1e-6);
  EXPECT_NEAR(0.0274893, comparisons[year][0].expected_, 1e-6);
  EXPECT_NEAR(88.3886, comparisons[year][0].score_, 1e-4);

  // age 6
  EXPECT_EQ("male", comparisons[year][5].category_);
  EXPECT_EQ(35, comparisons[year][5].length_);
  EXPECT_NEAR(0.08169356, comparisons[year][5].observed_, 1e-6);
  EXPECT_NEAR(0.0814484, comparisons[year][5].expected_, 1e-6);
  EXPECT_NEAR(294.428, comparisons[year][5].score_, 1e-3);

  // age 9
  EXPECT_EQ("male", comparisons[year][10].category_);
  EXPECT_EQ(40, comparisons[year][10].length_);
  EXPECT_NEAR(0.07832640, comparisons[year][10].observed_, 1e-6);
  EXPECT_NEAR(0.0770304, comparisons[year][10].expected_, 1e-6);
  EXPECT_NEAR(283.089, comparisons[year][10].score_, 1e-3);
}

}  // namespace age
} /* namespace niwa */

#endif /* TESTMODE */
