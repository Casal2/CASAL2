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

// Namespaces
namespace niwa {
namespace agelengths {
using ::testing::Return;
using ::testing::ReturnRef;

// classes
class MockTimeStepManager : public timesteps::Manager {
public:
  unsigned time_step_index_ = 0;
  unsigned current_time_step() const override final { return time_step_index_; }
};

class MockSchnute : public Schnute {
public:
  MockSchnute(ModelPtr model, Double y1, Double y2, Double tau1, Double tau2, Double a, Double b, bool by_length,
      Double cv_first, Double cv_last, vector<Double> time_step_proportions) : Schnute(model) {
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
};

/**
 * Test the results of our KnifeEdge are correct
 */
TEST(AgeLengths, Schnute) {
  MockTimeStepManager time_step_manager;

  MockManagers mock_managers;
  EXPECT_CALL(mock_managers, time_step()).WillRepeatedly(ReturnRef(time_step_manager));

  std::shared_ptr<MockModel> model = std::shared_ptr<MockModel>(new MockModel);
  EXPECT_CALL(*model.get(), min_age()).WillRepeatedly(Return(5));
  EXPECT_CALL(*model.get(), max_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(*model.get(), managers()).WillRepeatedly(ReturnRef(mock_managers));

  MockSchnute schnute(model, 24.5, 104.8, 1, 20, 0.131, 1.70, true, 1.5, 0.01, {0.0});


  EXPECT_DOUBLE_EQ(69.024873822523432, schnute.mean_length(1999, 5));
  EXPECT_DOUBLE_EQ(74.848134092163818, schnute.mean_length(1999, 6));
  EXPECT_DOUBLE_EQ(79.70651775663795, schnute.mean_length(1999, 7));
  EXPECT_DOUBLE_EQ(83.803556291745934, schnute.mean_length(1999, 8));
  EXPECT_DOUBLE_EQ(87.285326700186346, schnute.mean_length(1999, 9));
  EXPECT_DOUBLE_EQ(90.261388412893822, schnute.mean_length(1999, 10));


  ASSERT_NO_THROW(schnute.BuildCV(1999));
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(5));
  EXPECT_DOUBLE_EQ(1.0914274084458957, schnute.cv(6));
  EXPECT_DOUBLE_EQ(0.75055265570521079, schnute.cv(7));
  EXPECT_DOUBLE_EQ(0.46309553126356651, schnute.cv(8));
  EXPECT_DOUBLE_EQ(0.21880695525926219, schnute.cv(9));
  EXPECT_DOUBLE_EQ(0.010000000000000009, schnute.cv(10));

}

/**
 *
 */
TEST(AgeLengths, Schnute_BuildCV_ByLength_Proportion) {
  MockTimeStepManager time_step_manager;

  MockManagers mock_managers;
  EXPECT_CALL(mock_managers, time_step()).WillRepeatedly(ReturnRef(time_step_manager));

  std::shared_ptr<MockModel> model = std::shared_ptr<MockModel>(new MockModel);
  EXPECT_CALL(*model.get(), min_age()).WillRepeatedly(Return(5));
  EXPECT_CALL(*model.get(), max_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(*model.get(), managers()).WillRepeatedly(ReturnRef(mock_managers));

  MockSchnute schnute(model, 24.5, 104.8, 1, 20, 0.131, 1.70, true, 1.5, 7, {0.3});

  ASSERT_NO_THROW(schnute.BuildCV(1999));
  EXPECT_DOUBLE_EQ(0.0, schnute.cv(4));
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(5));
  EXPECT_DOUBLE_EQ(3.0010905968278787, schnute.cv(6));
  EXPECT_DOUBLE_EQ(4.2580033449542789, schnute.cv(7));
  EXPECT_DOUBLE_EQ(5.3206754587332945, schnute.cv(8));
  EXPECT_DOUBLE_EQ(6.2254832544081697, schnute.cv(9));
  EXPECT_DOUBLE_EQ(7.0, schnute.cv(10));
  EXPECT_DOUBLE_EQ(0.0, schnute.cv(11));

  EXPECT_DOUBLE_EQ(70.88858658180817, schnute.mean_length(1999, 5));
  EXPECT_DOUBLE_EQ(76.396278285380305, schnute.mean_length(1999, 6));
  EXPECT_DOUBLE_EQ(81.008050505923777, schnute.mean_length(1999, 7));
  EXPECT_DOUBLE_EQ(84.907129213795571, schnute.mean_length(1999, 8));
  EXPECT_DOUBLE_EQ(88.226983724973564, schnute.mean_length(1999, 9));
  EXPECT_DOUBLE_EQ(91.068783855484241, schnute.mean_length(1999, 10));
}



/**
 *
 */
TEST(AgeLengths, Schnute_BuildCV_ByLength_ProportionAndTimeStep) {
  MockTimeStepManager time_step_manager;
  time_step_manager.time_step_index_ = 1;

  MockManagers mock_managers;
  EXPECT_CALL(mock_managers, time_step()).WillRepeatedly(ReturnRef(time_step_manager));

  std::shared_ptr<MockModel> model = std::shared_ptr<MockModel>(new MockModel);
  EXPECT_CALL(*model.get(), min_age()).WillRepeatedly(Return(5));
  EXPECT_CALL(*model.get(), max_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(*model.get(), managers()).WillRepeatedly(ReturnRef(mock_managers));

  MockSchnute schnute(model, 24.5, 104.8, 1, 20, 0.131, 1.70, true, 0.2, 0.9, {0.25, 0.5});
  ASSERT_NO_THROW(schnute.BuildCV(1999));
  EXPECT_DOUBLE_EQ(0.0, schnute.cv(4));
  EXPECT_DOUBLE_EQ(0.20000000000000001, schnute.cv(5));
  EXPECT_DOUBLE_EQ(0.39049772657895609, schnute.cv(6));
  EXPECT_DOUBLE_EQ(0.55035970221265917, schnute.cv(7));
  EXPECT_DOUBLE_EQ(0.68573111126953545, schnute.cv(8));
  EXPECT_DOUBLE_EQ(0.80112907312632387, schnute.cv(9));
  EXPECT_DOUBLE_EQ(0.89999999999999991, schnute.cv(10));
  EXPECT_DOUBLE_EQ(0.0, schnute.cv(11));
}

/**
 *
 */
TEST(AgeLengths, Schnute_BuildCV_LinearInterpolation) {
  MockTimeStepManager time_step_manager;

  MockManagers mock_managers;
  EXPECT_CALL(mock_managers, time_step()).WillRepeatedly(ReturnRef(time_step_manager));

  std::shared_ptr<MockModel> model = std::shared_ptr<MockModel>(new MockModel);
  EXPECT_CALL(*model.get(), min_age()).WillRepeatedly(Return(5));
  EXPECT_CALL(*model.get(), max_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(*model.get(), managers()).WillRepeatedly(ReturnRef(mock_managers));

  MockSchnute schnute(model, 24.5, 104.8, 1, 20, 0.131, 1.70, false, 0.1, 0.9, {1.0});
  ASSERT_NO_THROW(schnute.BuildCV(1999));
  EXPECT_DOUBLE_EQ(0.0, schnute.cv(4));
  EXPECT_DOUBLE_EQ(0.10000000000000001, schnute.cv(5));
  EXPECT_DOUBLE_EQ(0.26000000000000001, schnute.cv(6));
  EXPECT_DOUBLE_EQ(0.42000000000000004, schnute.cv(7));
  EXPECT_DOUBLE_EQ(0.58000000000000007, schnute.cv(8));
  EXPECT_DOUBLE_EQ(0.73999999999999999, schnute.cv(9));
  EXPECT_DOUBLE_EQ(0.90000000000000002, schnute.cv(10));
  EXPECT_DOUBLE_EQ(0.0, schnute.cv(11));
}

} /* namespace agelengths */
} /* namespace niwa */


#endif
