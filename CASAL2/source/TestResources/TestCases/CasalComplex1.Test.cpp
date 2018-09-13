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
#include "Observations/Manager.h"
#include "Observations/Observation.h"
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
TEST_F(InternalEmptyModel, Model_CasalComplex1_BasicRun) {
  AddConfigurationLine(test_cases_casal_complex_1, "CasalComplex1.h", 31);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(86017.407059557605, obj_function.score());
}

/**
 *
 */
TEST_F(InternalEmptyModel, Model_CasalComplex1_Estimation) {
  AddConfigurationLine(test_cases_casal_complex_1, "CasalComplex1.h", 31);
  LoadConfiguration();

  model_->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(70082.72043536164, obj_function.score());
}

/**
 *
 */
TEST_F(InternalEmptyModel, Model_CasalComplex1_Simulation) {
  AddConfigurationLine(test_cases_casal_complex_1, "CasalComplex1.h", 31);
  LoadConfiguration();

  model_->Start(RunMode::kSimulation);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(0.0, obj_function.score());

  Observation* observation = model_->managers().observation()->GetObservation("chatTANage");
  if (!observation && observation->label() != "chatTANage")
    LOG_ERROR() << "Observation chatTANage could not be loaded for testing";

  map<unsigned, vector<obs::Comparison> >& comparisons = observation->comparisons();
  ASSERT_EQ(11u, comparisons.size());
  ASSERT_EQ(26u, comparisons[1992].size());

  for (auto iterator : comparisons) {
    double sum = 0.0;
    for (auto comparison : iterator.second)
      sum += comparison.observed_;
    EXPECT_DOUBLE_EQ(1.0, sum) << " for year " << iterator.first;
  }

  EXPECT_DOUBLE_EQ(1.0150837531311599e-011,  comparisons[1992][0].observed_);
  EXPECT_DOUBLE_EQ(0.010470482494025671, comparisons[1992][1].observed_);
  EXPECT_DOUBLE_EQ(0.012347042547332919,  comparisons[1992][2].observed_);
  EXPECT_DOUBLE_EQ(0.010111907418287399,  comparisons[1992][3].observed_);
  EXPECT_DOUBLE_EQ(0.015590386918144931,   comparisons[1992][4].observed_);
  EXPECT_DOUBLE_EQ(0.040143133477430946,  comparisons[1992][5].observed_);
  EXPECT_DOUBLE_EQ(0.053110701331827929,   comparisons[1992][6].observed_);
  EXPECT_DOUBLE_EQ(0.03141798580362324,   comparisons[1992][7].observed_);
  EXPECT_DOUBLE_EQ(0.044980767245313164,   comparisons[1992][8].observed_);
  EXPECT_DOUBLE_EQ(0.056026019079491715,  comparisons[1992][9].observed_);
}

} /* namespace testcases */
} /* namespace niwa */


#endif
