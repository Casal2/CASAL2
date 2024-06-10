/**
 * @file TwoSexModel.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 23/01/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * This file contains the Casal2 version of the two-sex model.
 */
#ifdef TESTMODE

#include "DerivedQuantities/Manager.h"
#include "Model/Model.h"
#include "ObjectiveFunction/ObjectiveFunction.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"
#include "TwoSexModel.h"

// Namespaces
namespace niwa {
namespace testcases {

using niwa::testfixtures::InternalEmptyModel;
using std::cout;
using std::endl;

/**
 *
 */
TEST_F(InternalEmptyModel, Model_TwoSex_BasicRun) {
  AddConfigurationLine(test_cases_two_sex_model_population, __FILE__, 27);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(2698.1334330594036, obj_function.score());
}

/**
 *
 */
TEST_F(InternalEmptyModel, Model_TwoSex_Estimation) {
  AddConfigurationLine(test_cases_two_sex_model_population, __FILE__, 27);
  LoadConfiguration();

  model_->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(1993.8041770617783, obj_function.score());
}

/**
 *
 */
TEST_F(InternalEmptyModel, Model_TwoSex_Foward_Projection) {
  AddConfigurationLine(test_cases_two_sex_model_population, __FILE__, 27);
  LoadConfiguration();

  model_->Start(RunMode::kProjection);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(0.0, obj_function.score());

  DerivedQuantity* dq = model_->managers()->derived_quantity()->GetDerivedQuantity("abundance");
  EXPECT_DOUBLE_EQ(1725780.7680744524, dq->GetValue(2010));
  EXPECT_DOUBLE_EQ(1746451.4919400334, dq->GetValue(2011));
  EXPECT_DOUBLE_EQ(1765234.8648326413, dq->GetValue(2012));
}

} /* namespace testcases */
} /* namespace niwa */

#endif
