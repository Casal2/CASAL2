/**
 * @file KnifeEdge.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifdef TESTMODE
#include "KnifeEdge.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TestResources/MockClasses/Model.h"

// Namespaces
namespace niwa {

using ::testing::Return;

/**
 * Test the results of our KnifeEdge are correct
 */
TEST(Selectivities, KnifeEdge) {
  MockModel model;
  EXPECT_CALL(model, min_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(model, max_age()).WillRepeatedly(Return(20));

  niwa::selectivities::KnifeEdge knife_edge(&model);
  knife_edge.parameters().Add(PARAM_LABEL, "unit_test_knife_edge", __FILE__, __LINE__);
  knife_edge.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  knife_edge.parameters().Add(PARAM_E, "15", __FILE__, __LINE__);
  knife_edge.Validate();
  knife_edge.Build();

  EXPECT_DOUBLE_EQ(0.0, knife_edge.GetResult(9, nullptr)); // Below model->min_age()
  EXPECT_DOUBLE_EQ(0.0, knife_edge.GetResult(10, nullptr)); // At model->min_age()
  EXPECT_DOUBLE_EQ(0.0, knife_edge.GetResult(11, nullptr));
  EXPECT_DOUBLE_EQ(0.0, knife_edge.GetResult(12, nullptr));
  EXPECT_DOUBLE_EQ(0.0, knife_edge.GetResult(13, nullptr));
  EXPECT_DOUBLE_EQ(0.0, knife_edge.GetResult(14, nullptr));
  EXPECT_DOUBLE_EQ(1.0, knife_edge.GetResult(15, nullptr));
  EXPECT_DOUBLE_EQ(1.0, knife_edge.GetResult(16, nullptr));
  EXPECT_DOUBLE_EQ(1.0, knife_edge.GetResult(17, nullptr));
  EXPECT_DOUBLE_EQ(1.0, knife_edge.GetResult(18, nullptr));
  EXPECT_DOUBLE_EQ(1.0, knife_edge.GetResult(19, nullptr));
  EXPECT_DOUBLE_EQ(1.0, knife_edge.GetResult(20, nullptr)); // At model->max_age()
  EXPECT_DOUBLE_EQ(0.0, knife_edge.GetResult(21, nullptr)); // This is above model->max_age()
  EXPECT_DOUBLE_EQ(0.0, knife_edge.GetResult(22, nullptr));
  EXPECT_DOUBLE_EQ(0.0, knife_edge.GetResult(23, nullptr));
}

} /* namespace niwa */
#endif
