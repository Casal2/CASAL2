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
  std::shared_ptr<MockModel> model = std::shared_ptr<MockModel>(new MockModel);
  EXPECT_CALL(*model.get(), min_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(*model.get(), max_age()).WillRepeatedly(Return(20));

  niwa::selectivities::AllValuesBounded all_values_bounded(model);

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

  EXPECT_DOUBLE_EQ(0.0,  all_values_bounded.GetResult(9)); // Below model->min_age()
  EXPECT_DOUBLE_EQ(0.0,  all_values_bounded.GetResult(10)); // At model->min_age()
  EXPECT_DOUBLE_EQ(0.0,  all_values_bounded.GetResult(11));
  EXPECT_DOUBLE_EQ(2.0,  all_values_bounded.GetResult(12));
  EXPECT_DOUBLE_EQ(4.0,  all_values_bounded.GetResult(13));
  EXPECT_DOUBLE_EQ(6.0, all_values_bounded.GetResult(14));
  EXPECT_DOUBLE_EQ(8.0, all_values_bounded.GetResult(15));
  EXPECT_DOUBLE_EQ(10.0, all_values_bounded.GetResult(16));
  EXPECT_DOUBLE_EQ(12.0,  all_values_bounded.GetResult(17));
  EXPECT_DOUBLE_EQ(12.0,  all_values_bounded.GetResult(18));
  EXPECT_DOUBLE_EQ(12.0,  all_values_bounded.GetResult(19));
  EXPECT_DOUBLE_EQ(12.0,  all_values_bounded.GetResult(20)); // At model->max_age()
  EXPECT_DOUBLE_EQ(0.0,  all_values_bounded.GetResult(21)); // This is above model->max_age()
  EXPECT_DOUBLE_EQ(0.0,  all_values_bounded.GetResult(22));
  EXPECT_DOUBLE_EQ(0.0,  all_values_bounded.GetResult(23));
}

}

#endif /* ifdef TESTMODE */
