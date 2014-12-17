/**
 * @file CasalComplex3.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 25/03/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

#include "CasalComplex3.h"

#include "Model/Model.h"
#include "ObjectiveFunction/ObjectiveFunction.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"

// Namespaces
namespace niwa {
namespace testcases {

using std::cout;
using std::endl;
using niwa::testfixtures::InternalEmptyModel;

/**
 *
 */
TEST_F(InternalEmptyModel, Model_CasalComplex3_BasicRun) {
  AddConfigurationLine(test_cases_casal_complex_3, "CasalComplex3.h", 31);
  LoadConfiguration();

  ModelPtr model = Model::Instance();
  model->Start(RunMode::kBasic);

  ObjectiveFunction& obj_function = ObjectiveFunction::Instance();
  EXPECT_DOUBLE_EQ(1560.8575036493282, obj_function.score());
}

/**
 *
 */
TEST_F(InternalEmptyModel, Model_CasalComplex3_Estimation) {
  AddConfigurationLine(test_cases_casal_complex_3, "CasalComplex3.h", 31);
  LoadConfiguration();

  ModelPtr model = Model::Instance();
  model->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = ObjectiveFunction::Instance();
  EXPECT_NEAR(7.1292288397386461, obj_function.score(), 1e-5);
}

} /* namespace testcases */
} /* namespace niwa */


#endif
