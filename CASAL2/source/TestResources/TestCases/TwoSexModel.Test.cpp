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
 * This file contains the CASAL2 version of the two-sex model.
 */
#ifdef TESTMODE

#include "TwoSexModel.h"

#include "DerivedQuantities/Manager.h"
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
  EXPECT_DOUBLE_EQ(1993.8041773625964, obj_function.score());
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

  DerivedQuantity* dq = model_->managers().derived_quantity()->GetDerivedQuantity("abundance");
  EXPECT_DOUBLE_EQ(1773708.1521442106, dq->GetValue(2010));
  EXPECT_DOUBLE_EQ(1794657.5435387883, dq->GetValue(2011));
  EXPECT_DOUBLE_EQ(1813872.7774264852, dq->GetValue(2012));
}

} /* namespace testcases */
} /* namespace niwa */


#endif
