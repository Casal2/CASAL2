/**
 * @file Inverse.Test.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 11, 2016
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

const string non_transformed_esimate_blocks =
R"(
@estimate
parameter catchability[CPUEq].q
lower_bound 1e-10
upper_bound 1e-1
type uniform

@estimate
parameter selectivity[FishingSel].a50
lower_bound 1
upper_bound 20
type uniform

@estimate FishingSel.ato95
parameter selectivity[FishingSel].ato95
lower_bound 0.01
upper_bound 50
type uniform


)";

const string estimable_transformation_inverse_jacobian =
R"(
  @parameter_transformation inverse_r0
  type inverse
  parameters process[Recruitment].r0
  prior_applies_to_restored_parameters true

  @estimate inverse_r0
  type uniform_log
  parameter parameter_transformation[inverse_r0].inverse_parameter
  lower_bound 1e-10 
  upper_bound 1e-05
)";

const string estimable_transformation_inverse =
R"(
  @parameter_transformation inverse_r0
  type inverse
  parameters process[Recruitment].r0
  prior_applies_to_restored_parameters false

  @estimate inverse_r0
  type uniform_log
  parameter parameter_transformation[inverse_r0].inverse_parameter
  lower_bound 1e-10 
  upper_bound 1e-05
)";
/**
 *
 */
TEST_F(InternalEmptyModel, EstimableTransformations_Inverse_jacobian) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates, "TestResources/Models/TwoSexNoEstimates.h", 28);
  AddConfigurationLine(non_transformed_esimate_blocks, __FILE__, 22);
  AddConfigurationLine(estimable_transformation_inverse_jacobian, __FILE__, 22);
  LoadConfiguration();
  MockObjects       mock_objects(model_);
  model_->Start(RunMode::kBasic);
  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_NEAR(2670.5076467884028, obj_function.score(), 1e-4);
  Double*  est_r0 = model_->objects().GetAddressable("process[Recruitment].r0");
  EXPECT_NEAR(997386, *est_r0, 1e-2);
}
/**
 *
 */
TEST_F(InternalEmptyModel, EstimableTransformations_Inverse) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates, "TestResources/Models/TwoSexNoEstimates.h", 28);
  AddConfigurationLine(non_transformed_esimate_blocks, __FILE__, 22);
  AddConfigurationLine(estimable_transformation_inverse, __FILE__, 22);
  LoadConfiguration();
  MockObjects       mock_objects(model_);
  model_->Start(RunMode::kBasic);
  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_NEAR(2670.5076467884028, obj_function.score(), 1e-4);
  Double*  est_r0 = model_->objects().GetAddressable("process[Recruitment].r0");
  EXPECT_NEAR(997386, *est_r0, 1e-2);
}
/**
 *
 
TEST_F(InternalEmptyModel, EstimableTransformations_Inverse_jacobian_est) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates, "TestResources/Models/TwoSexNoEstimates.h", 28);
  AddConfigurationLine(non_transformed_esimate_blocks, __FILE__, 22);
  AddConfigurationLine(estimable_transformation_inverse_jacobian, __FILE__, 51);
  LoadConfiguration();

  model_->Start(RunMode::kEstimation);
  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_NEAR(1890.0450191917889, obj_function.score(), 1e-4);
  Double*  est_r0 = model_->objects().GetAddressable("process[Recruitment].r0");
  EXPECT_NEAR(997386, *est_r0, 1e-2);
}
*/

} /* namespace estimatetransformations */
} /* namespace niwa */
#endif
