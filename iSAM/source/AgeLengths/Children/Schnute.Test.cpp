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

class MockTimeStepManager : public timesteps::Manager {
public:
  unsigned time_step_index_ = 0;
  unsigned current_time_step() const override final { return time_step_index_; }
};

class SchnuteTest : public Schnute {
public:
  SchnuteTest(ModelPtr model, Double y1, Double y2, Double tau1, Double tau2, Double a, Double b, bool by_length,
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
  EXPECT_CALL(*model.get(), age_spread()).WillRepeatedly(Return(11));
  EXPECT_CALL(*model.get(), managers()).WillRepeatedly(ReturnRef(mock_managers));

  SchnuteTest schnute(model, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, true, 1.5, 0.0, {1.0});

  EXPECT_DOUBLE_EQ(2.002204803472166, schnute.mean_length(1999, 4));
  EXPECT_DOUBLE_EQ(2.0022196182093586, schnute.mean_length(1999, 5));
  EXPECT_DOUBLE_EQ(2.0022197180284138, schnute.mean_length(1999, 6));
  EXPECT_DOUBLE_EQ(2.0022197187009891, schnute.mean_length(1999, 7));
  EXPECT_DOUBLE_EQ(2.0022197187055211, schnute.mean_length(1999, 8));
  EXPECT_DOUBLE_EQ(2.0022197187055517, schnute.mean_length(1999, 9));
  EXPECT_DOUBLE_EQ(2.0022197187055517, schnute.mean_length(1999, 10));
  EXPECT_DOUBLE_EQ(2.0022197187055517, schnute.mean_length(1999, 11));

  ASSERT_NO_THROW(schnute.BuildCV(1999));
  EXPECT_DOUBLE_EQ(0.0, schnute.cv(4));
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(5));
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(6));
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(7));
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(8));
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(9));
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(10));
  EXPECT_DOUBLE_EQ(0.0, schnute.cv(11));
}

/**
 *
 */
TEST(AgeLengths, Schnute_BuildCV_ByLength) {
  MockTimeStepManager time_step_manager;

  MockManagers mock_managers;
  EXPECT_CALL(mock_managers, time_step()).WillRepeatedly(ReturnRef(time_step_manager));

  std::shared_ptr<MockModel> model = std::shared_ptr<MockModel>(new MockModel);
  EXPECT_CALL(*model.get(), min_age()).WillRepeatedly(Return(5));
  EXPECT_CALL(*model.get(), max_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(*model.get(), age_spread()).WillRepeatedly(Return(11));
  EXPECT_CALL(*model.get(), managers()).WillRepeatedly(ReturnRef(mock_managers));

  SchnuteTest schnute(model, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, true, 1.5, 7.0, {1.0});
  ASSERT_NO_THROW(schnute.BuildCV(1999));
  EXPECT_DOUBLE_EQ(0.0, schnute.cv(4));
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(5));
  EXPECT_DOUBLE_EQ(6.9629412981662986, schnute.cv(6));
  EXPECT_DOUBLE_EQ(6.9997502975167949, schnute.cv(7));
  EXPECT_DOUBLE_EQ(6.9999983230025951, schnute.cv(8));
  EXPECT_DOUBLE_EQ(7.0, schnute.cv(9));
  EXPECT_DOUBLE_EQ(7.0, schnute.cv(10));
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
  EXPECT_CALL(*model.get(), age_spread()).WillRepeatedly(Return(11));
  EXPECT_CALL(*model.get(), managers()).WillRepeatedly(ReturnRef(mock_managers));

  SchnuteTest schnute(model, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, false, 1.5, 12.5, {1.0});
  ASSERT_NO_THROW(schnute.BuildCV(1999));
  EXPECT_DOUBLE_EQ(0.0, schnute.cv(4));
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(5));
  EXPECT_DOUBLE_EQ(3.7000000000000002, schnute.cv(6));
  EXPECT_DOUBLE_EQ(5.9000000000000004, schnute.cv(7));
  EXPECT_DOUBLE_EQ(8.0999999999999996, schnute.cv(8));
  EXPECT_DOUBLE_EQ(10.300000000000001, schnute.cv(9));
  EXPECT_DOUBLE_EQ(12.5, schnute.cv(10));
  EXPECT_DOUBLE_EQ(0.0, schnute.cv(11));
}

/**
 *
 */
TEST(AgeLengths, Schnute_BuildCV_LinearInterpolation_Proportion) {
  MockTimeStepManager time_step_manager;

  MockManagers mock_managers;
  EXPECT_CALL(mock_managers, time_step()).WillRepeatedly(ReturnRef(time_step_manager));

  std::shared_ptr<MockModel> model = std::shared_ptr<MockModel>(new MockModel);
  EXPECT_CALL(*model.get(), min_age()).WillRepeatedly(Return(5));
  EXPECT_CALL(*model.get(), max_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(*model.get(), age_spread()).WillRepeatedly(Return(11));
  EXPECT_CALL(*model.get(), managers()).WillRepeatedly(ReturnRef(mock_managers));

  SchnuteTest schnute(model, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, true, 1.5, 12.5, {0.5});
  ASSERT_NO_THROW(schnute.BuildCV(1999));
  EXPECT_DOUBLE_EQ(0.0, schnute.cv(4));
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(5));
  EXPECT_DOUBLE_EQ(12.425882696108761, schnute.cv(6));
  EXPECT_DOUBLE_EQ(12.499500603486316, schnute.cv(7));
  EXPECT_DOUBLE_EQ(12.499996636402226, schnute.cv(8));
  EXPECT_DOUBLE_EQ(12.499999976059803, schnute.cv(9));
  EXPECT_DOUBLE_EQ(12.5, schnute.cv(10));
  EXPECT_DOUBLE_EQ(0.0, schnute.cv(11));
}

/**
 *
 */
TEST(AgeLengths, Schnute_BuildCV_LinearInterpolation_ProportionAndTimeStep) {
  MockTimeStepManager time_step_manager;
  time_step_manager.time_step_index_ = 1;

  MockManagers mock_managers;
  EXPECT_CALL(mock_managers, time_step()).WillRepeatedly(ReturnRef(time_step_manager));

  std::shared_ptr<MockModel> model = std::shared_ptr<MockModel>(new MockModel);
  EXPECT_CALL(*model.get(), min_age()).WillRepeatedly(Return(5));
  EXPECT_CALL(*model.get(), max_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(*model.get(), age_spread()).WillRepeatedly(Return(11));
  EXPECT_CALL(*model.get(), managers()).WillRepeatedly(ReturnRef(mock_managers));

  SchnuteTest schnute(model, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, true, 1.5, 12.5, {0.5, 0.25});
  ASSERT_NO_THROW(schnute.BuildCV(1999));
  EXPECT_DOUBLE_EQ(0.0, schnute.cv(4));
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(5));
  EXPECT_DOUBLE_EQ(12.425882976579283, schnute.cv(6));
  EXPECT_DOUBLE_EQ(12.499500604032709, schnute.cv(7));
  EXPECT_DOUBLE_EQ(12.499996635682395, schnute.cv(8));
  EXPECT_DOUBLE_EQ(12.499999977136815, schnute.cv(9));
  EXPECT_DOUBLE_EQ(12.5, schnute.cv(10));
  EXPECT_DOUBLE_EQ(0.0, schnute.cv(11));
}

} /* namespace agelengths */
} /* namespace niwa */


#endif
