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
//#ifdef TESTMODE
#include "KnifeEdge.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TestResources/MockClasses/Model.h"

// Namespaces
namespace isam {

using ::testing::Return;

/**
 * Test the results of our KnifeEdge are correct
 */
TEST(Selectivities, KnifeEdge) {
  boost::shared_ptr<MockModel> model = boost::shared_ptr<MockModel>(new MockModel);
  EXPECT_CALL(*model.get(), min_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(*model.get(), max_age()).WillRepeatedly(Return(20));

  isam::selectivities::KnifeEdge knife_edge(model);
  knife_edge.parameters().Add(PARAM_LABEL, "unit_test_knife_edge", __FILE__, __LINE__);
  knife_edge.parameters().Add(PARAM_E, "15", __FILE__, __LINE__);
  knife_edge.Validate();
  knife_edge.Build();

  EXPECT_DOUBLE_EQ(0.0, knife_edge.GetResult(9)); // Below model->min_age()
  EXPECT_DOUBLE_EQ(0.0, knife_edge.GetResult(10)); // At model->min_age()
  EXPECT_DOUBLE_EQ(0.0, knife_edge.GetResult(11));
  EXPECT_DOUBLE_EQ(0.0, knife_edge.GetResult(12));
  EXPECT_DOUBLE_EQ(0.0, knife_edge.GetResult(13));
  EXPECT_DOUBLE_EQ(0.0, knife_edge.GetResult(14));
  EXPECT_DOUBLE_EQ(1.0, knife_edge.GetResult(15));
  EXPECT_DOUBLE_EQ(1.0, knife_edge.GetResult(16));
  EXPECT_DOUBLE_EQ(1.0, knife_edge.GetResult(17));
  EXPECT_DOUBLE_EQ(1.0, knife_edge.GetResult(18));
  EXPECT_DOUBLE_EQ(1.0, knife_edge.GetResult(19));
  EXPECT_DOUBLE_EQ(1.0, knife_edge.GetResult(20)); // At model->max_age()
  EXPECT_DOUBLE_EQ(0.0, knife_edge.GetResult(21)); // This is above model->max_age()
  EXPECT_DOUBLE_EQ(0.0, knife_edge.GetResult(22));
  EXPECT_DOUBLE_EQ(0.0, knife_edge.GetResult(23));
}

} /* namespace isam */
//#endif
