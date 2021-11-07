/**
 * @file Simplex.Test.cpp
 * @author C.Marsh
 * @date 5/11/2021
 * @section LICENSE
 *
 * Copyright NIWA Science �2016 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// headers
#include "../../TestResources/Models/TwoSexNoEstimates.h"
#include "../../TestResources/TestFixtures/InternalEmptyModel.h"
#include "../../BaseClasses/Object.h"
#include "../../Model/Model.h"
#include "../../Model/Objects.h"
#include "../../Model/Objects.Mock.h"
#include "../../ObjectiveFunction/ObjectiveFunction.h"

#include "../../TestResources/MockClasses/Model.h"

namespace niwa {
namespace addressabletransformations {
using niwa::testfixtures::InternalEmptyModel;

/*
* This first set of tests check bounds sanity checks and also
* checks the simplex work on string maps using process[Recruitment].proportions parameter must sum = 1
*/
const string transform_prop_with_estimate =
R"(
  @parameter_transformation simp_prop
  type simplex
  parameters process[Recruitment].proportions
  sum_to_one true
  prior_applies_to_restored_parameters true

  @estimate prop_simplex
  type uniform
  parameter parameter_transformation[simp_prop].simplex
  lower_bound -10 -10
  upper_bound 10 10
)";
const string transform_prop_with_estimate_expect_true =
R"(
  @parameter_transformation simp_prop
  type simplex
  parameters process[Recruitment].proportions
  sum_to_one true
  prior_applies_to_restored_parameters true

  @estimate prop_simplex
  type uniform
  parameter parameter_transformation[simp_prop].simplex
  lower_bound -10 
  upper_bound 10
)";
const string transform_prop_with_estimate_expect_fail =
R"(
  @parameter_transformation simp_prop
  type simplex
  parameters process[Recruitment].proportions
  sum_to_one true
  prior_applies_to_restored_parameters false

  @estimate prop_simplex
  type uniform
  parameter parameter_transformation[simp_prop].simplex
  lower_bound -10 -10
  upper_bound 10 10
)";
const string transform_propr_expect_prior_false =
R"(
  @parameter_transformation simp_prop
  type simplex
  parameters process[Recruitment].proportions
  sum_to_one true
  prior_applies_to_restored_parameters false

  @estimate prop_simplex
  type uniform
  parameter parameter_transformation[simp_prop].simplex
  lower_bound -10 
  upper_bound 10 
)";
/**
 * if prior_applies_to_restored_parameters == true
 * users need to supply n bounds, if false they need to specify n-1 bounds
 */
TEST_F(InternalEmptyModel, AddressableTransformations_simplex_fail_estimate_bounds_prior_false) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates, "TestResources/Models/TwoSexNoEstimates.h", 28);
  AddConfigurationLine(transform_prop_with_estimate_expect_fail, __FILE__, 51);
  LoadConfiguration();
  EXPECT_THROW(model_->Start(RunMode::kBasic), std::string);
}
/**
 * if prior_applies_to_restored_parameters == true
 * users need to supply n bounds, if false they need to specify n-1 bounds
 */
TEST_F(InternalEmptyModel, AddressableTransformations_simplex_fail_estimate_bounds_prior_true) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates, "TestResources/Models/TwoSexNoEstimates.h", 28);
  AddConfigurationLine(transform_prop_with_estimate_expect_true, __FILE__, 51);
  LoadConfiguration();
  EXPECT_THROW(model_->Start(RunMode::kBasic), std::string);
}
/**
 *
 */
TEST_F(InternalEmptyModel, AddressableTransformations_simplex_with_jacobian) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates, "TestResources/Models/TwoSexNoEstimates.h", 28);
  AddConfigurationLine(transform_prop_with_estimate, __FILE__, 51);
  LoadConfiguration();
  EXPECT_NO_THROW(model_->Start(RunMode::kBasic));
}
/**
 *
 */
TEST_F(InternalEmptyModel, AddressableTransformations_simplex_with_no_jacobian) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates, "TestResources/Models/TwoSexNoEstimates.h", 28);
  AddConfigurationLine(transform_propr_expect_prior_false, __FILE__, 51);
  LoadConfiguration();
  EXPECT_NO_THROW(model_->Start(RunMode::kBasic));
}

} /* namespace estimatetransformations */
} /* namespace niwa */
#endif
