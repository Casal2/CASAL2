/**
 * @file Inverse.Test.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 11, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// headers
#include "../../Model/Models/Age.h"
#include "../../ObjectiveFunction/ObjectiveFunction.h"
#include "../../TestResources/Models/TwoSex.h"
#include "../../TestResources/Models/TwoSexWithDLib.h"
#include "../../TestResources/Models/TwoSexWithDeSolver.h"
#include "../../TestResources/TestFixtures/InternalEmptyModel.h"

namespace niwa {
namespace estimatetransformations {
using niwa::testfixtures::InternalEmptyModel;

const string estimate_transformation_inverse =
    R"(
@estimate_transformation x
type inverse
estimate_label recruitment.r0
transform_with_jacobian true

)";

/**
 *
 */
TEST_F(InternalEmptyModel, EstimateTransformations_Inverse) {
  AddConfigurationLine(testresources::models::two_sex, "TestResources/Models/TwoSex.h", 28);
  AddConfigurationLine(estimate_transformation_inverse, __FILE__, 22);
  LoadConfiguration();

  model_->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_NEAR(1977.895725058263, obj_function.score(), 1e-4);
}

/**
 *
 */
const string estimate_transformation_inverse_no_bounds =
    R"(
@estimate_transformation x
type inverse
estimate_label recruitment.r0
)";

/**
 *
 */
TEST_F(InternalEmptyModel, EstimateTransformations_Inverse_NoBounds) {
  AddConfigurationLine(testresources::models::two_sex, "TestResources/Models/TwoSex.h", 28);
  AddConfigurationLine(estimate_transformation_inverse_no_bounds, __FILE__, 51);
  LoadConfiguration();

  model_->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_NEAR(1977.895725058263, obj_function.score(), 1e-4);
}

/**
 * This test yields difference results between windows and Linux (I believe this a model issue not a code error) So moved it out currently until someone looks at it
 * TODO: understand this issue
 */
/*
TEST_F(InternalEmptyModel, EstimateTransformations_Inverse_NoBounds_With_DLib_Minimiser) {
  AddConfigurationLine(testresources::models::two_sex_with_dlib, "TestResources/Models/TwoSexWithDLib.h", 28);
  AddConfigurationLine(estimate_transformation_inverse_no_bounds, __FILE__, 51);
  LoadConfiguration();

  model_->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(1977.6906102868909, obj_function.score());
}
*/

/**
 *
 */
TEST_F(InternalEmptyModel, EstimateTransformations_Inverse_NoBounds_With_DeSolver_Minimiser) {
  AddConfigurationLine(testresources::models::two_sex_with_de_solver, "TestResources/Models/TwoSexWithDeSolver.h", 28);
  AddConfigurationLine(estimate_transformation_inverse_no_bounds, __FILE__, 51);
  LoadConfiguration();

  model_->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(2220.7769862967702, obj_function.score());
}

} /* namespace estimatetransformations */
} /* namespace niwa */
#endif
