/**
 * @file Estimate.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 29/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifdef TESTMODE

// Headers
#include <iostream>

#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Estimates/Manager.h"
#include "Model/Model.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"
#include "TestResources/Models/TwoSexNoEstimates.h"
#include "TestResources/Models/TwoSexNoEstimatesAllValuesMortality.h"

// Namespaces
namespace niwa {
namespace estimates {

using std::cout;
using std::endl;
using niwa::testfixtures::InternalEmptyModel;

const string estimate_single_target =
R"(
@estimate e1
parameter selectivity[FishingSel].a50
lower_bound 1
upper_bound 20
type beta
mu 0.3
sigma 0.05
a 0
b 10
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Estimates_Single_Target) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates, "TestResources/Models/TwoSexNoEstimates.h", 28);
  AddConfigurationLine(estimate_single_target, __FILE__, 35);
  LoadConfiguration();

  model_->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(1726.6295023192379, obj_function.score());

  Estimate* estimate = model_->managers().estimate()->GetEstimate("selectivity[FishingSel].a50");
  if (!estimate)
    LOG_FATAL() << "!estimate";
  EXPECT_DOUBLE_EQ(estimate->value(), 7.2724038656178385);

  // Check results
  estimate->set_value(1.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), -2476.5137933614251);
  estimate->set_value(2.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), -2367.250113991935);
  estimate->set_value(3.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), -2230.4867585646953);
  estimate->set_value(4.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), -2066.4915312599851);
  estimate->set_value(5.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), -1868.5574163359895);
}

/**
 *
 */
const string estimate_multiple_defined_targets_vector =
R"(
@estimate e1
parameter selectivity[av].v(21:25)
lower_bound 1 1 1 1 1
upper_bound 20 20 20 20 20
type lognormal
mu 3 5 3 5 3
cv 4 5 4 6 4
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Estimates_Multiple_Defined_Targets_Vector) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates_all_values_mortality, "TestResources/Models/TwoSexNoEstimatesAllValuesMortality.h", 28);
  AddConfigurationLine(estimate_multiple_defined_targets_vector, __FILE__, 83);
  LoadConfiguration();

  model_->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(35171.333647143554, obj_function.score());

  Estimate* estimate = model_->managers().estimate()->GetEstimate("selectivity[av].v(21)");
  if (!estimate)
    LOG_FATAL() << "!estimate";
  EXPECT_DOUBLE_EQ(estimate->value(), 1.0000667295269956);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 0.017919754558039881);

  estimate = model_->managers().estimate()->GetEstimate("selectivity[av].v(25)");
  if (!estimate)
    LOG_FATAL() << "!estimate";
  EXPECT_DOUBLE_EQ(estimate->value(), 1.000014483955731);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 0.017861646655730232);
}

/**
 *
 */
const string estimate_multiple_defined_targets_unsigned_map =
R"(
@estimate
parameter process[Fishing].catches(2000:2002)
type lognormal
lower_bound 28000 24000 47000
upper_bound 29000 25000 48000
mu 3 5 7
cv 4 6 8
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Estimates_Multiple_Defined_Targets_Unsigned_Map) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates, "TestResources/Models/TwoSexNoEstimates.h", 28);
  AddConfigurationLine(estimate_multiple_defined_targets_unsigned_map, __FILE__, 124);
  LoadConfiguration();

  model_->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(2764.0521558776445, obj_function.score());

  Estimate* estimate = model_->managers().estimate()->GetEstimate("process[Fishing].catches(2000)");
  if (!estimate)
    LOG_FATAL() << "!estimate";
  EXPECT_DOUBLE_EQ(estimate->value(), 28323.203463000002);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 29.966307896614428);

  estimate = model_->managers().estimate()->GetEstimate("process[Fishing].catches(2001)");
  if (!estimate)
    LOG_FATAL() << "!estimate";
  EXPECT_DOUBLE_EQ(estimate->value(), 24207.464203);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 24.757322431641612);

  estimate = model_->managers().estimate()->GetEstimate("process[Fishing].catches(2002)");
  if (!estimate)
    LOG_FATAL() << "!estimate";
  EXPECT_DOUBLE_EQ(estimate->value(), 47279);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 25.007985625485446);

}

/**
 *
 */
const string estimate_multiple_defined_targets_string_map =
R"(
@estimate recruitment.proportions
type lognormal
parameter process[Recruitment].proportions(immature.male,immature.female)
lower_bound 0.4 0.4
upper_bound 0.6 0.6
mu 1 2
cv 3 4
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Estimates_Multiple_Defined_Targets_String_Map) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates, "TestResources/Models/TwoSexNoEstimates.h", 28);
  AddConfigurationLine(estimate_multiple_defined_targets_string_map, __FILE__, 171);
  LoadConfiguration();

  model_->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(2682.979986309937, obj_function.score());

  Estimate* estimate = model_->managers().estimate()->GetEstimate("process[Recruitment].proportions(immature.male)");
  if (!estimate)
    LOG_FATAL() << "!estimate";
  EXPECT_DOUBLE_EQ(estimate->value(), 0.5);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), -0.64756858783643167);

  estimate = model_->managers().estimate()->GetEstimate("process[Recruitment].proportions(immature.female)");
  if (!estimate)
    LOG_FATAL() << "!estimate";
  EXPECT_DOUBLE_EQ(estimate->value(), 0.5);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), -0.69298502612944257);
}

/**
 *
 */
const string estimate_all_targets_vector =
R"(
@estimate e3
parameter selectivity[av].v
lower_bound 1 1 2 2 3 3 4 4 5 5 6 6 7 7 8 8 9 9 10 10 11 11 12 12 13 13 14 14 15 15 16 16 17 17 18 18 19 19 20 20 21 21 22 22 23 23 24 24 25 25
upper_bound 2 2 3 3 4 4 5 5 6 6 7 7 8 8 9 9 10 10 11 11 12 12 13 13 14 14 15 15 16 16 17 17 18 18 19 19 20 20 21 21 22 22 23 23 24 24 25 25 26 26
type lognormal
mu 3 5 3 5 3 3 5 3 5 3 3 5 3 5 3 3 5 3 5 3 3 5 3 5 3 3 5 3 5 3 3 5 3 5 3 3 5 3 5 3 3 5 3 5 3 3 5 3 5 3
cv 4 5 4 6 4 4 5 4 6 4 4 5 4 6 4 4 5 4 6 4 4 5 4 6 4 4 5 4 6 4 4 5 4 6 4 4 5 4 6 4 4 5 4 6 4 4 5 4 6 4
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Estimates_All_Targets_Vector) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates_all_values_mortality, "TestResources/Models/TwoSexNoEstimatesAllValuesMortality.h", 28);
  AddConfigurationLine(estimate_all_targets_vector, __FILE__, 212);
  LoadConfiguration();

  model_->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(39989.151448120341, obj_function.score());

  Estimate* estimate = model_->managers().estimate()->GetEstimate("selectivity[av].v(10)");
  if (!estimate)
    LOG_FATAL() << "!estimate";
  EXPECT_DOUBLE_EQ(estimate->value(), 5.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 2.2650530751015676);

  estimate = model_->managers().estimate()->GetEstimate("selectivity[av].v(40)");
  if (!estimate)
    LOG_FATAL() << "!estimate";
  EXPECT_DOUBLE_EQ(estimate->value(), 20.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 4.9335998153292859);
}

/**
 *
 */
const string estimate_all_targets_unsigned_map =
R"(
@estimate 
parameter process[Fishing].catches
type lognormal
lower_bound 1800 14000 28000 24000 47000 58000 82000 115000 113000 119000
upper_bound 1900 15000 29000 25000 48000 59000 83000 116000 114000 120000
mu 3 5 3 5 7 1 2 3 4 5
cv 4 6 4 6 8 5 6 7 8 9
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Estimates_All_Targets_Unsigned_Map) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates, "TestResources/Models/TwoSexNoEstimates.h", 28);
  AddConfigurationLine(estimate_all_targets_unsigned_map, __FILE__, 253);
  LoadConfiguration();

  model_->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(2963.7305613725566, obj_function.score());

  Estimate* estimate = model_->managers().estimate()->GetEstimate("process[Fishing].catches(1998)");
  if (!estimate)
    LOG_FATAL() << "!estimate";
  EXPECT_DOUBLE_EQ(estimate->value(), 1849.153714);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 18.371135369472036);

  estimate = model_->managers().estimate()->GetEstimate("process[Fishing].catches(2006)");
  if (!estimate)
    LOG_FATAL() << "!estimate";
  EXPECT_DOUBLE_EQ(estimate->value(), 113852.472257);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 29.892453993650498);
}

/**
 *
 */
const string estimate_all_targets_string_map =
R"(
@estimate recruitment.proportions
type lognormal
parameter process[Recruitment].proportions
lower_bound 0.4 0.4
upper_bound 0.6 0.6
mu 1 2
cv 3 4
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Estimates_All_Targets_String_Map) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates, "TestResources/Models/TwoSexNoEstimates.h", 28);
  AddConfigurationLine(estimate_all_targets_string_map, __FILE__, 293);
  LoadConfiguration();

  model_->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(2682.979986309937, obj_function.score());

  Estimate* estimate = model_->managers().estimate()->GetEstimate("process[Recruitment].proportions(immature.male)");
  if (!estimate)
    LOG_FATAL() << "!estimate";
  EXPECT_DOUBLE_EQ(estimate->value(), 0.5);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), -0.64756858783643167);

  estimate = model_->managers().estimate()->GetEstimate("process[Recruitment].proportions(immature.female)");
  if (!estimate)
    LOG_FATAL() << "!estimate";
  EXPECT_DOUBLE_EQ(estimate->value(), 0.5);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), -0.69298502612944257);
}

} /* namespace estimates */
} /* namespace niwa */
#endif
