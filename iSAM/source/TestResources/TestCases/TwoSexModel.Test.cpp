/**
 * @file TwoSexModel.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 23/01/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This file contains the iSAM version of the two-sex model.
 */
#ifdef TESTMODE

#include "TwoSexModel.h"

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
TEST_F(InternalEmptyModel, Model_TwoSex_BasicRun) {
  AddConfigurationLine(test_cases_two_sex_model_population, __FILE__, __LINE__);
  AddConfigurationLine(test_cases_two_sex_model_estimation, __FILE__, __LINE__);
  LoadConfiguration();

  ModelPtr model = Model::Instance();
  model->Start(RunMode::kBasic);

  ObjectiveFunction& obj_function = ObjectiveFunction::Instance();
  EXPECT_DOUBLE_EQ(2698.1334330594036, obj_function.score());
}

/**
 *
 */
TEST_F(InternalEmptyModel, Model_TwoSex_Estimation) {
  AddConfigurationLine(test_cases_two_sex_model_population, __FILE__, __LINE__);
  AddConfigurationLine(test_cases_two_sex_model_estimation, __FILE__, __LINE__);
  LoadConfiguration();

  ModelPtr model = Model::Instance();
  model->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = ObjectiveFunction::Instance();
  EXPECT_DOUBLE_EQ(1993.8041771768321, obj_function.score());
}

} /* namespace testcases */
} /* namespace isam */


#endif
