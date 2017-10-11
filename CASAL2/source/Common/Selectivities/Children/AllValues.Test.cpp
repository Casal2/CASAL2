/**
 * @file AllValues.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifdef TESTMODE

// Headers
#include "AllValues.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <boost/lexical_cast.hpp>

#include "TestResources/MockClasses/Model.h"
#include "Common/Utilities/PartitionType.h"

// Namespaces
namespace niwa {

using ::testing::Return;
using ::testing::ReturnRef;
/**
 * Test the results of our KnifeEdge are correct
 */
TEST(Selectivities, AllValues_Age) {
  MockModel model;
  EXPECT_CALL(model, min_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(model, max_age()).WillRepeatedly(Return(20));
  EXPECT_CALL(model, age_spread()).WillRepeatedly(Return(11));
  EXPECT_CALL(model, partition_type()).WillRepeatedly(Return(PartitionType::kAge));

  niwa::selectivities::AllValues all_values(&model);

  vector<string> v;
  for (unsigned i = 0; i < 11; ++i)
    v.push_back(boost::lexical_cast<string>(i + 1));

  all_values.parameters().Add(PARAM_LABEL, "unit_test_all_values", __FILE__, __LINE__);
  all_values.parameters().Add(PARAM_V, v, __FILE__, __LINE__);
  all_values.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  all_values.Validate();
  all_values.Build();

  EXPECT_DOUBLE_EQ(0.0,  all_values.GetAgeResult(9, nullptr)); // Below model->min_age()
  EXPECT_DOUBLE_EQ(1.0,  all_values.GetAgeResult(10, nullptr)); // At model->min_age()
  EXPECT_DOUBLE_EQ(2.0,  all_values.GetAgeResult(11, nullptr));
  EXPECT_DOUBLE_EQ(3.0,  all_values.GetAgeResult(12, nullptr));
  EXPECT_DOUBLE_EQ(4.0,  all_values.GetAgeResult(13, nullptr));
  EXPECT_DOUBLE_EQ(5.0, all_values.GetAgeResult(14, nullptr));
  EXPECT_DOUBLE_EQ(6.0, all_values.GetAgeResult(15, nullptr));
  EXPECT_DOUBLE_EQ(7.0, all_values.GetAgeResult(16, nullptr));
  EXPECT_DOUBLE_EQ(8.0, all_values.GetAgeResult(17, nullptr));
  EXPECT_DOUBLE_EQ(9.0, all_values.GetAgeResult(18, nullptr));
  EXPECT_DOUBLE_EQ(10.0, all_values.GetAgeResult(19, nullptr));
  EXPECT_DOUBLE_EQ(11.0,  all_values.GetAgeResult(20, nullptr)); // At model->max_age()
  EXPECT_DOUBLE_EQ(0.0,  all_values.GetAgeResult(21, nullptr)); // This is above model->max_age()
  EXPECT_DOUBLE_EQ(0.0,  all_values.GetAgeResult(22, nullptr));
  EXPECT_DOUBLE_EQ(0.0,  all_values.GetAgeResult(23, nullptr));
}

TEST(Selectivities, AllValues_Length) {
  MockModel model;
  vector<unsigned> lengths = {10, 20, 30, 40, 50, 60, 120};
  //ON_CALL(model, length_bins()).WillByDefault(ReturnRef(lengths));

  EXPECT_CALL(model, length_bins()).WillRepeatedly(ReturnRef(lengths));
  EXPECT_CALL(model, partition_type()).WillRepeatedly(Return(PartitionType::kLength));

  niwa::selectivities::AllValues all_values(&model);
  vector<string> v = {"0.0","0.1","0.2","0.3","0.1","0.5", "0.0325"};
  vector<double> values = { 0.0 ,0.1 , 0.2 , 0.3 , 0.1 , 0.5 , 0.0325};

  all_values.parameters().Add(PARAM_LABEL, "unit_test_all_values", __FILE__, __LINE__);
  all_values.parameters().Add(PARAM_V, v, __FILE__, __LINE__);
  all_values.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  all_values.Validate();
  all_values.Build();


  for (unsigned i = 0; i < v.size(); ++i) {
    EXPECT_DOUBLE_EQ(values[i],  all_values.GetLengthResult(i));
  }


}
}

#endif /* ifdef TESTMODE */
