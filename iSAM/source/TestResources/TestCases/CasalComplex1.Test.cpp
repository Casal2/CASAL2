/**
 * @file CasalComplex1.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 14/03/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

#include "CasalComplex1.h"

#include "Model/Model.h"
#include "ObjectiveFunction/ObjectiveFunction.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"

// Namespaces
namespace isam {
namespace testcases {

using std::cout;
using std::endl;
using isam::testfixtures::InternalEmptyModel;

/**
 *
 */
TEST_F(InternalEmptyModel, Model_CasalComplex1_BasicRun) {
  AddConfigurationLine(test_cases_casal_complex_1, "CasalComplex1.h", 31);
  LoadConfiguration();

  ModelPtr model = Model::Instance();
  model->Start(RunMode::kBasic);

  ObjectiveFunction& obj_function = ObjectiveFunction::Instance();
  EXPECT_DOUBLE_EQ(16420.52830668812, obj_function.score());
}

/**
 *
 */
TEST_F(InternalEmptyModel, Model_CasalComplex1_Estimation) {
  AddConfigurationLine(test_cases_casal_complex_1, "CasalComplex1.h", 31);
  LoadConfiguration();

  ModelPtr model = Model::Instance();
  model->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = ObjectiveFunction::Instance();
  EXPECT_DOUBLE_EQ(463.46264220283479, obj_function.score());
}

} /* namespace testcases */
} /* namespace isam */


#endif
