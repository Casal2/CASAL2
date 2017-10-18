/**
 * @file Constant.Test.cpp
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

// Headers
#include "Constant.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TestResources/MockClasses/Model.h"

namespace niwa {
using ::testing::Return;
using ::testing::ReturnRef;

/**
 * Test the return override for the Constant selectivity is
 * working properly.
 */
TEST(Selectivities, Constant_age) {

  MockModel model;
  EXPECT_CALL(model, partition_type()).WillRepeatedly(Return(PartitionType::kAge));
  EXPECT_CALL(model, min_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(model, max_age()).WillRepeatedly(Return(20));
  niwa::selectivities::Constant constant(&model);
  constant.parameters().Add(PARAM_LABEL, "unit_test_constant", __FILE__, __LINE__);
  constant.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  constant.parameters().Add(PARAM_C, "83", __FILE__, __LINE__);
  constant.Validate();
  constant.Build();

  EXPECT_DOUBLE_EQ(83.0, constant.GetAgeResult(0, nullptr));
  EXPECT_DOUBLE_EQ(83.0, constant.GetAgeResult(1, nullptr));
  EXPECT_DOUBLE_EQ(83.0, constant.GetAgeResult(2, nullptr));
  EXPECT_DOUBLE_EQ(83.0, constant.GetAgeResult(3, nullptr));
  EXPECT_DOUBLE_EQ(83.0, constant.GetAgeResult(4, nullptr));
}

TEST(Selectivities, Constant_Length) {
  MockModel model;
  vector<unsigned> lengths = {10, 20, 30, 40, 50, 60, 120};
  EXPECT_CALL(model, length_bins()).WillRepeatedly(ReturnRef(lengths));
  EXPECT_CALL(model, partition_type()).WillRepeatedly(Return(PartitionType::kLength));
  niwa::selectivities::Constant constant(&model);
  constant.parameters().Add(PARAM_LABEL, "unit_test_all_values", __FILE__, __LINE__);
  constant.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  constant.parameters().Add(PARAM_C, "22", __FILE__, __LINE__);
  constant.Validate();
  constant.Build();


  for (unsigned i = 0; i < lengths.size(); ++i) {
    EXPECT_DOUBLE_EQ(22,  constant.GetLengthResult(i));
  }

}

} /* namespace niwa */
#endif
