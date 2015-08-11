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

class TimeStepManager : public timesteps::Manager {
  unsigned current_time_step() const override final { return 0; }
};

class SchnuteTest : public Schnute {
public:
  SchnuteTest(ModelPtr model, Double y1, Double y2, Double tau1, Double tau2, Double a, Double b, bool by_length) : Schnute(model) {
    y1_ = y1;
    y2_ = y2;
    tau1_ = tau1;
    tau2_ = tau2;
    a_ = a;
    b_ = b;
    by_length_ = by_length;
  };
};

/**
 * Test the results of our KnifeEdge are correct
 */
TEST(AgeLengths, Schnute) {
  TimeStepManager time_step_manager;

  MockManagers mock_managers;
  EXPECT_CALL(mock_managers, time_step()).WillRepeatedly(ReturnRef(time_step_manager));

  std::shared_ptr<MockModel> model = std::shared_ptr<MockModel>(new MockModel);
  EXPECT_CALL(*model.get(), min_age()).WillRepeatedly(Return(5));
  EXPECT_CALL(*model.get(), max_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(*model.get(), age_spread()).WillRepeatedly(Return(11));
  EXPECT_CALL(*model.get(), managers()).WillRepeatedly(ReturnRef(mock_managers));

  SchnuteTest schnute(model, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, true);
  ASSERT_NO_THROW(schnute.BuildCV(1999));

  EXPECT_DOUBLE_EQ(0.0, schnute.cv(4));
  EXPECT_DOUBLE_EQ(0.0, schnute.cv(5));
  EXPECT_DOUBLE_EQ(0.0, schnute.cv(6));
  EXPECT_DOUBLE_EQ(0.0, schnute.cv(7));
  EXPECT_DOUBLE_EQ(0.0, schnute.cv(8));
  EXPECT_DOUBLE_EQ(0.0, schnute.cv(9));
  EXPECT_DOUBLE_EQ(0.0, schnute.cv(10));
  EXPECT_DOUBLE_EQ(0.0, schnute.cv(11));
}

} /* namespace agelengths */
} /* namespace niwa */


#endif
