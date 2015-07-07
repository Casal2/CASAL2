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

/**
 *
 */
TEST_F(InternalEmptyModel, Model_CasalComplex1_Simulation) {
  AddConfigurationLine(test_cases_casal_complex_1, "CasalComplex1.h", 31);
  LoadConfiguration();

  ModelPtr model = Model::Instance();
  model->Start(RunMode::kSimulation);

  ObjectiveFunction& obj_function = ObjectiveFunction::Instance();
  EXPECT_DOUBLE_EQ(0.0, obj_function.score());

  ObservationPtr observation = observations::Manager::Instance().GetObservation("chatTANage");
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

  EXPECT_DOUBLE_EQ(0.0066381041682021229,  comparisons[1992][0].observed_);
  EXPECT_DOUBLE_EQ(0.0070833196771564336, comparisons[1992][1].observed_);
  EXPECT_DOUBLE_EQ(0.0057998649850546837,  comparisons[1992][2].observed_);
  EXPECT_DOUBLE_EQ(0.01090770432991572,  comparisons[1992][3].observed_);
  EXPECT_DOUBLE_EQ(0.027242617586312013,   comparisons[1992][4].observed_);
  EXPECT_DOUBLE_EQ(0.040261942218864102,  comparisons[1992][5].observed_);
  EXPECT_DOUBLE_EQ(0.027889541540686152,   comparisons[1992][6].observed_);
  EXPECT_DOUBLE_EQ(0.043455271082630184,   comparisons[1992][7].observed_);
  EXPECT_DOUBLE_EQ(0.059710394387489041,   comparisons[1992][8].observed_);
  EXPECT_DOUBLE_EQ(0.071124937102585217,  comparisons[1992][9].observed_);
}

} /* namespace testcases */
} /* namespace niwa */


#endif
