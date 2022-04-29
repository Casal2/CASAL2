/**
 * @file KnifeEdge.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifdef TESTMODE
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../TestResources/MockClasses/Model.h"
#include "../../TestResources/MockClasses/ModelLength.h"
#include "KnifeEdge.h"

// Namespaces
namespace niwa {

using ::testing::Return;
using ::testing::ReturnRef;

/**
 * Test the results of our KnifeEdge are correct
 */
TEST(Selectivities, KnifeEdge_Age) {
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  EXPECT_CALL(*model, min_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(*model, max_age()).WillRepeatedly(Return(20));
  EXPECT_CALL(*model, age_spread()).WillRepeatedly(Return(11));
  EXPECT_CALL(*model, partition_type()).WillRepeatedly(Return(PartitionType::kAge));

  niwa::selectivities::KnifeEdge knife_edge(model);
  knife_edge.parameters().Add(PARAM_LABEL, "unit_test_knife_edge", __FILE__, __LINE__);
  knife_edge.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  knife_edge.parameters().Add(PARAM_E, "15", __FILE__, __LINE__);
  knife_edge.Validate();
  knife_edge.Build();

  ASSERT_THROW(knife_edge.GetAgeResult(9, nullptr), std::string);  // Below model->min_age()
  EXPECT_DOUBLE_EQ(0.0, knife_edge.GetAgeResult(10, nullptr));     // At model->min_age()
  EXPECT_DOUBLE_EQ(0.0, knife_edge.GetAgeResult(11, nullptr));
  EXPECT_DOUBLE_EQ(0.0, knife_edge.GetAgeResult(12, nullptr));
  EXPECT_DOUBLE_EQ(0.0, knife_edge.GetAgeResult(13, nullptr));
  EXPECT_DOUBLE_EQ(0.0, knife_edge.GetAgeResult(14, nullptr));
  EXPECT_DOUBLE_EQ(1.0, knife_edge.GetAgeResult(15, nullptr));
  EXPECT_DOUBLE_EQ(1.0, knife_edge.GetAgeResult(16, nullptr));
  EXPECT_DOUBLE_EQ(1.0, knife_edge.GetAgeResult(17, nullptr));
  EXPECT_DOUBLE_EQ(1.0, knife_edge.GetAgeResult(18, nullptr));
  EXPECT_DOUBLE_EQ(1.0, knife_edge.GetAgeResult(19, nullptr));
  EXPECT_DOUBLE_EQ(1.0, knife_edge.GetAgeResult(20, nullptr));      // At model->max_age()
  ASSERT_THROW(knife_edge.GetAgeResult(21, nullptr), std::string);  // This is above model->max_age()
}

TEST(Selectivities, KnifeEdge_Length) {
  shared_ptr<MockModelLength> model = shared_ptr<MockModelLength>(new MockModelLength());
  vector<double> lengths = {10, 20, 30, 40, 50, 60, 120};
  EXPECT_CALL(*model, length_bin_mid_points()).WillRepeatedly(ReturnRef(lengths));
  EXPECT_CALL(*model, partition_type()).WillRepeatedly(Return(PartitionType::kLength));

  niwa::selectivities::KnifeEdge knife_edge(model);

  vector<double> expected_values = {0, 0, 0, 1, 1, 1, 1};

  knife_edge.parameters().Add(PARAM_LABEL, "unit_test_knife_edge", __FILE__, __LINE__);
  knife_edge.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  knife_edge.parameters().Add(PARAM_E, "35", __FILE__, __LINE__);
  knife_edge.Validate();
  knife_edge.Build();

  for (unsigned i = 0; i < lengths.size(); ++i) {
    EXPECT_DOUBLE_EQ(expected_values[i], knife_edge.GetLengthResult(i));
  }
}

} /* namespace niwa */
#endif
