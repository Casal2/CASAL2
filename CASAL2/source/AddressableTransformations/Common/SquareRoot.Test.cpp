/**
 * @file SquareRoot.Test.cpp
 * @author A. Dunn
 * @date Nov, 2022
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 */
#ifdef TESTMODE

// headers
#include "../../BaseClasses/Object.h"
#include "../../Model/Model.h"
#include "../../Model/Objects.Mock.h"
#include "../../Model/Objects.h"
#include "../../ObjectiveFunction/ObjectiveFunction.h"
#include "../../TestResources/Models/TwoSexNoEstimates.h"
#include "../../TestResources/TestFixtures/InternalEmptyModel.h"

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

const string estimable_transformation_sqrt_jacobian =
    R"(
  @parameter_transformation sqrt_r0
  type sqrt
  parameters process[Recruitment].r0
  prior_applies_to_restored_parameters true

  @estimate sqrt_r0
  type uniform_log
  parameter parameter_transformation[sqrt_r0].sqrt_parameter
  lower_bound 314
  upper_bound 99000
)";

const string estimable_transformation_sqrt =
    R"(
  @parameter_transformation sqrt_r0
  type sqrt
  parameters process[Recruitment].r0
  prior_applies_to_restored_parameters false

  @estimate sqrt_r0
  type uniform_log
  parameter parameter_transformation[sqrt_r0].sqrt_parameter
  lower_bound 314
  upper_bound 99000
)";
/**
 *
 */
TEST_F(InternalEmptyModel, EstimableTransformations_sqrt_jacobian) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates, "TestResources/Models/TwoSexNoEstimates.h", 28);
  AddConfigurationLine(non_transformed_esimate_blocks, __FILE__, 22);
  AddConfigurationLine(estimable_transformation_sqrt_jacobian, __FILE__, 22);
  LoadConfiguration();
  MockObjects mock_objects(model_);
  model_->Start(RunMode::kBasic);
  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_NEAR(2690.5338393110942, obj_function.score(), 1e-4);
  Double* est_r0 = model_->objects().GetAddressable("process[Recruitment].r0");
  EXPECT_NEAR(997386, *est_r0, 1e-2);
}

/**
 *
 */
TEST_F(InternalEmptyModel, EstimableTransformations_sqrt) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates, "TestResources/Models/TwoSexNoEstimates.h", 28);
  AddConfigurationLine(non_transformed_esimate_blocks, __FILE__, 22);
  AddConfigurationLine(estimable_transformation_sqrt, __FILE__, 22);
  LoadConfiguration();
  MockObjects mock_objects(model_);
  model_->Start(RunMode::kBasic);
  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_NEAR(2691.2269864916539, obj_function.score(), 1e-4);
  Double* est_r0 = model_->objects().GetAddressable("process[Recruitment].r0");
  EXPECT_NEAR(997386, *est_r0, 1e-2);
}

}  // namespace addressabletransformations
} /* namespace niwa */
#endif
