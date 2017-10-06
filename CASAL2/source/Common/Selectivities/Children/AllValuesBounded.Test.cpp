/**
 * @file AllValuesBounded.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 22/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifdef TESTMODE

// Headers
#include "AllValuesBounded.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <boost/lexical_cast.hpp>

#include "TestResources/MockClasses/Model.h"

// Namespaces
namespace niwa {

using ::testing::Return;

/**
 * Test the results of our selectivity are correct
 */
TEST(Selectivities, AllValuesBounded) {
  MockModel model;
  EXPECT_CALL(model, min_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(model, max_age()).WillRepeatedly(Return(20));

  niwa::selectivities::AllValuesBounded all_values_bounded(&model);

  vector<string> v;
  for (unsigned i = 0; i < 6; ++i)
    v.push_back(boost::lexical_cast<string>( (i + 1) * 2 ));

  all_values_bounded.parameters().Add(PARAM_LABEL, "unit_test_all_values_bounded", __FILE__, __LINE__);
  all_values_bounded.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  all_values_bounded.parameters().Add(PARAM_L, "12",  __FILE__, __LINE__);
  all_values_bounded.parameters().Add(PARAM_H, "17", __FILE__, __LINE__);
  all_values_bounded.parameters().Add(PARAM_V, v, __FILE__, __LINE__);
  all_values_bounded.Validate();
  all_values_bounded.Build();

  EXPECT_DOUBLE_EQ(0.0,  all_values_bounded.GetAgeResult(9, nullptr)); // Below model->min_age()
  EXPECT_DOUBLE_EQ(0.0,  all_values_bounded.GetAgeResult(10, nullptr)); // At model->min_age()
  EXPECT_DOUBLE_EQ(0.0,  all_values_bounded.GetAgeResult(11, nullptr));
  EXPECT_DOUBLE_EQ(2.0,  all_values_bounded.GetAgeResult(12, nullptr));
  EXPECT_DOUBLE_EQ(4.0,  all_values_bounded.GetAgeResult(13, nullptr));
  EXPECT_DOUBLE_EQ(6.0, all_values_bounded.GetAgeResult(14, nullptr));
  EXPECT_DOUBLE_EQ(8.0, all_values_bounded.GetAgeResult(15, nullptr));
  EXPECT_DOUBLE_EQ(10.0, all_values_bounded.GetAgeResult(16, nullptr));
  EXPECT_DOUBLE_EQ(12.0,  all_values_bounded.GetAgeResult(17, nullptr));
  EXPECT_DOUBLE_EQ(12.0,  all_values_bounded.GetAgeResult(18, nullptr));
  EXPECT_DOUBLE_EQ(12.0,  all_values_bounded.GetAgeResult(19, nullptr));
  EXPECT_DOUBLE_EQ(12.0,  all_values_bounded.GetAgeResult(20, nullptr)); // At model->max_age()
  EXPECT_DOUBLE_EQ(0.0,  all_values_bounded.GetAgeResult(21, nullptr)); // This is above model->max_age()
  EXPECT_DOUBLE_EQ(0.0,  all_values_bounded.GetAgeResult(22, nullptr));
  EXPECT_DOUBLE_EQ(0.0,  all_values_bounded.GetAgeResult(23, nullptr));
}

}

#endif /* ifdef TESTMODE */
