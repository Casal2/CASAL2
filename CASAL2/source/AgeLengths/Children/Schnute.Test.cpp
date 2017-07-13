/**
 * @file Schnute.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 19/08/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "Schnute.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TestResources/MockClasses/Managers.h"
#include "TestResources/MockClasses/Model.h"
#include "TimeSteps/Manager.h"

// Namespaces
namespace niwa {
namespace agelengths {
using ::testing::Return;
using ::testing::ReturnRef;

// classes
class MockTimeStepManager : public timesteps::Manager {
public:
  MockTimeStepManager() = default;
  unsigned time_step_index_ = 0;
  unsigned current_time_step() const override final { return time_step_index_; }
};

class MockSchnute : public Schnute {
public:
  MockSchnute(Model& model, Double y1, Double y2, Double tau1, Double tau2, Double a, Double b, bool by_length,
      Double cv_first, Double cv_last, vector<Double> time_step_proportions) : Schnute(&model) {
    y1_ = y1;
    y2_ = y2;
    tau1_ = tau1;
    tau2_ = tau2;
    a_ = a;
    b_ = b;
    by_length_ = by_length;
    cv_first_ = cv_first;
    cv_last_ = cv_last;
    time_step_proportions_ = time_step_proportions;
  };


  void MockBuildCV() {
    this->BuildCV();
  }
};

/**
 * Test the results of our Schnute are correct
 */
TEST(AgeLengths, Schnute) {
  MockModel model;
  MockManagers mock_managers(&model);
  MockTimeStepManager time_step_manager;
  time_step_manager.time_step_index_ = 0;
  vector<string> time_steps = {"0", "1", "2"};

  EXPECT_CALL(model, min_age()).WillRepeatedly(Return(5));
  EXPECT_CALL(model, max_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(model, start_year()).WillRepeatedly(Return(1990));
  EXPECT_CALL(model, final_year()).WillRepeatedly(Return(1999));
  EXPECT_CALL(model, managers()).WillRepeatedly(ReturnRef(mock_managers));
  EXPECT_CALL(model, time_steps()).WillRepeatedly(ReturnRef(time_steps));

  EXPECT_CALL(mock_managers, time_step()).WillRepeatedly(Return(&time_step_manager));

  MockSchnute schnute(model, 24.5, 104.8, 1, 20, 0.131, 1.70, true, 1.5, 0.01, {0.0});

  EXPECT_DOUBLE_EQ(69.024873822523432, schnute.mean_length(1990, 5));
  EXPECT_DOUBLE_EQ(74.848134092163818, schnute.mean_length(1990, 6));
  EXPECT_DOUBLE_EQ(79.70651775663795, schnute.mean_length(1990, 7));
  EXPECT_DOUBLE_EQ(83.803556291745934, schnute.mean_length(1990, 8));
  EXPECT_DOUBLE_EQ(87.285326700186346, schnute.mean_length(1990, 9));
  EXPECT_DOUBLE_EQ(90.261388412893822, schnute.mean_length(1990, 10));

  ASSERT_NO_THROW(schnute.MockBuildCV());
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(1990,5, 0));
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(1990,6, 0));
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(1990,7, 0));
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(1990,8, 0));
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(1990,9, 0));
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(1990,10, 0));
}

/**
 *
 */
TEST(AgeLengths, Schnute_BuildCV_ByLength_Proportion) {
  MockModel model;
  MockManagers mock_managers(&model);
  MockTimeStepManager time_step_manager;
  time_step_manager.time_step_index_ = 0;
  EXPECT_CALL(mock_managers, time_step()).WillRepeatedly(Return(&time_step_manager));
  vector<string> time_steps = {"0", "1", "2"};

  EXPECT_CALL(model, min_age()).WillRepeatedly(Return(5));
  EXPECT_CALL(model, max_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(model, start_year()).WillRepeatedly(Return(1990));
  EXPECT_CALL(model, final_year()).WillRepeatedly(Return(1999));
  EXPECT_CALL(model, managers()).WillRepeatedly(ReturnRef(mock_managers));
  EXPECT_CALL(model, time_steps()).WillRepeatedly(ReturnRef(time_steps));

  MockSchnute schnute(model, 24.5, 104.8, 1, 20, 0.131, 1.70, true, 1.5, 7, {0.3});

  ASSERT_NO_THROW(schnute.MockBuildCV());

  EXPECT_DOUBLE_EQ(0.0,                 schnute.cv(1990,4, 0));
  EXPECT_DOUBLE_EQ(1.5,                 schnute.cv(1990,5, 0));
  EXPECT_DOUBLE_EQ(1.5,  schnute.cv(1990,6, 0));
  EXPECT_DOUBLE_EQ(1.5,  schnute.cv(1990,7, 0));
  EXPECT_DOUBLE_EQ(1.5,  schnute.cv(1990,8, 0));
  EXPECT_DOUBLE_EQ(1.5,  schnute.cv(1990,9, 0));
  EXPECT_DOUBLE_EQ(1.5,                 schnute.cv(1990,10, 0));
  EXPECT_DOUBLE_EQ(0.0,                 schnute.cv(1990,11, 0));

  EXPECT_DOUBLE_EQ(70.88858658180817,  schnute.mean_length(1990, 5));
  EXPECT_DOUBLE_EQ(76.396278285380305, schnute.mean_length(1990, 6));
  EXPECT_DOUBLE_EQ(81.008050505923777, schnute.mean_length(1990, 7));
  EXPECT_DOUBLE_EQ(84.907129213795571, schnute.mean_length(1990, 8));
  EXPECT_DOUBLE_EQ(88.226983724973564, schnute.mean_length(1990, 9));
  EXPECT_DOUBLE_EQ(91.068783855484241, schnute.mean_length(1990, 10));
}



/**
 *
 */
TEST(AgeLengths, Schnute_BuildCV_ByLength_ProportionAndTimeStep) {
  MockModel model;
  MockManagers mock_managers(&model);
  MockTimeStepManager time_step_manager;
  time_step_manager.time_step_index_ = 1;
  vector<string> time_steps = {"0", "1", "2"};

  EXPECT_CALL(mock_managers, time_step()).WillRepeatedly(Return(&time_step_manager));

  EXPECT_CALL(model, min_age()).WillRepeatedly(Return(5));
  EXPECT_CALL(model, max_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(model, start_year()).WillRepeatedly(Return(1990));
  EXPECT_CALL(model, final_year()).WillRepeatedly(Return(1999));
  EXPECT_CALL(model, managers()).WillRepeatedly(ReturnRef(mock_managers));
  EXPECT_CALL(model, time_steps()).WillRepeatedly(ReturnRef(time_steps));

  MockSchnute schnute(model, 24.5, 104.8, 1, 20, 0.131, 1.70, true, 0.2, 0.9, {0.25, 0.5});
  ASSERT_NO_THROW(schnute.MockBuildCV());
  EXPECT_DOUBLE_EQ(0.0,                 schnute.cv(1990, 4, 1));
  EXPECT_DOUBLE_EQ(0.20000000000000001, schnute.cv(1990, 5, 1));
  EXPECT_DOUBLE_EQ(0.20000000000000001, schnute.cv(1990, 6, 1));
  EXPECT_DOUBLE_EQ(0.20000000000000001, schnute.cv(1990, 7, 1));
  EXPECT_DOUBLE_EQ(0.20000000000000001, schnute.cv(1990, 8, 1));
  EXPECT_DOUBLE_EQ(0.20000000000000001, schnute.cv(1990, 9, 1));
  EXPECT_DOUBLE_EQ(0.20000000000000001, schnute.cv(1990, 10, 1));
  EXPECT_DOUBLE_EQ(0.0,                 schnute.cv(1990, 11, 1));
}

/**
 *  Since I made the change to BuildCV() to identify if the cv_last->was_defined(); the test doesn't recognise what parameters have been populated
 *  This component will be tested elsewhere-> TestModels/ORH has cv_last and cv_first by_length. So if that passe the test I am happy that this funcitonality works
 */
TEST(AgeLengths, Schnute_BuildCV_LinearInterpolation) {
  MockModel model;
  MockManagers mock_managers(&model);
  MockTimeStepManager time_step_manager;
  time_step_manager.time_step_index_ = 0;
  vector<string> time_steps = {"0", "1", "2"};

  EXPECT_CALL(mock_managers, time_step()).WillRepeatedly(Return(&time_step_manager));

  EXPECT_CALL(model, min_age()).WillRepeatedly(Return(5));
  EXPECT_CALL(model, max_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(model, start_year()).WillRepeatedly(Return(1990));
  EXPECT_CALL(model, final_year()).WillRepeatedly(Return(1999));
  EXPECT_CALL(model, managers()).WillRepeatedly(ReturnRef(mock_managers));
  EXPECT_CALL(model, time_steps()).WillRepeatedly(ReturnRef(time_steps));

  MockSchnute schnute(model, 24.5, 104.8, 1, 20, 0.131, 1.70, false, 0.1, 0.9, {1.0});
  ASSERT_NO_THROW(schnute.MockBuildCV());
  EXPECT_DOUBLE_EQ(0.0,                 schnute.cv(1990, 4, 0));
  EXPECT_DOUBLE_EQ(0.10000000000000001, schnute.cv(1990, 5, 0));
  EXPECT_DOUBLE_EQ(0.10000000000000001, schnute.cv(1990, 6, 0));
  EXPECT_DOUBLE_EQ(0.10000000000000001, schnute.cv(1990, 7, 0));
  EXPECT_DOUBLE_EQ(0.10000000000000001, schnute.cv(1990, 8, 0));
  EXPECT_DOUBLE_EQ(0.10000000000000001, schnute.cv(1990, 9, 0));
  EXPECT_DOUBLE_EQ(0.10000000000000001, schnute.cv(1990, 10, 0));
  EXPECT_DOUBLE_EQ(0.0,                 schnute.cv(1990, 11, 0));
}

} /* namespace agelengths */
} /* namespace niwa */


#endif
