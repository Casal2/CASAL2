/**
 * @file Log.Test.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 11, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 */
#ifdef TESTMODE

// headers
#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Model/Model.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"
#include "TestResources/Models/TwoSex.h"
#include "TestResources/Models/TwoSexWithDLib.h"
#include "TestResources/Models/TwoSexWithDeSolver.h"

namespace niwa {
namespace estimatetransformations {
using niwa::testfixtures::InternalEmptyModel;

const string estimate_transformation_log =
R"(
@estimate_transformation log_r0
type log
estimate_label recruitment.r0
)";

/**
 *
 */
TEST_F(InternalEmptyModel, EstimateTransformations_Log) {
  AddConfigurationLine(testresources::models::two_sex, "TestResources/Models/TwoSex.h", 28);
  AddConfigurationLine(estimate_transformation_log, __FILE__, 22);
  LoadConfiguration();

  model_->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(1977.8942177618578, obj_function.score());
}

/**
 *
 */
const string estimate_transformation_log_no_bounds =
R"(
@estimate_transformation log_r0
type log
estimate_label recruitment.r0
)";

/**
 *
 */
TEST_F(InternalEmptyModel, EstimateTransformations_Log_NoBounds) {
  AddConfigurationLine(testresources::models::two_sex, "TestResources/Models/TwoSex.h", 28);
  AddConfigurationLine(estimate_transformation_log_no_bounds, __FILE__, 50);
  LoadConfiguration();

  model_->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(1977.8942177618578, obj_function.score());
}

/**
 *
 */
TEST_F(InternalEmptyModel, EstimateTransformations_Log_With_DLib_Minimiser) {
  AddConfigurationLine(testresources::models::two_sex_with_dlib, "TestResources/Models/TwoSexWithDLib.h", 28);
  AddConfigurationLine(estimate_transformation_log_no_bounds, __FILE__, 22);
  LoadConfiguration();

  model_->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_NEAR(1977.6906102869461, obj_function.score(), 1e-5);
}

/**
 *
 */
TEST_F(InternalEmptyModel, EstimateTransformations_Log_With_DeSolver_Minimiser) {
  AddConfigurationLine(testresources::models::two_sex_with_de_solver, "TestResources/Models/TwoSexWithDeSolver.h", 28);
  AddConfigurationLine(estimate_transformation_log_no_bounds, __FILE__, 22);
  LoadConfiguration();

  model_->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(2214.974747990268, obj_function.score());
}

} /* namespace estimatetransformations */
} /* namespace niwa */
#endif
