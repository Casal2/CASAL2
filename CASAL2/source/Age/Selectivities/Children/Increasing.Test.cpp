/**
 * @file Increasing.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 22/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifdef TESTMODE

// Headers
#include "Increasing.h"

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
TEST(Selectivities, Increasing) {
  MockModel model;
  EXPECT_CALL(model, min_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(model, max_age()).WillRepeatedly(Return(20));

  niwa::selectivities::Increasing increasing(&model);

  vector<string> v;
  for (unsigned i = 12; i <= 18; ++i)
    v.push_back(boost::lexical_cast<string>(1.0 / (i * 1.5)));

  increasing.parameters().Add(PARAM_LABEL, "unit_test_increasing", __FILE__, __LINE__);
  increasing.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  increasing.parameters().Add(PARAM_L, "12",  __FILE__, __LINE__);
  increasing.parameters().Add(PARAM_H, "18",  __FILE__, __LINE__);
  increasing.parameters().Add(PARAM_V, v,  __FILE__, __LINE__);
  increasing.Validate();
  increasing.Build();

  EXPECT_DOUBLE_EQ(0.0,                       increasing.GetResult(9, nullptr)); // Below model->min_age()
  EXPECT_DOUBLE_EQ(0.0,                       increasing.GetResult(10, nullptr)); // At model->min_age()
  EXPECT_DOUBLE_EQ(0.0,                       increasing.GetResult(11, nullptr));
  EXPECT_DOUBLE_EQ(0.055555555555555552,      increasing.GetResult(12, nullptr));
  EXPECT_DOUBLE_EQ(0.055555555555555552,      increasing.GetResult(13, nullptr));
  EXPECT_DOUBLE_EQ(0.10398860398860399,       increasing.GetResult(14, nullptr));
  EXPECT_DOUBLE_EQ(0.14665581332247998,       increasing.GetResult(15, nullptr));
  EXPECT_DOUBLE_EQ(0.18458222161925864,       increasing.GetResult(16, nullptr));
  EXPECT_DOUBLE_EQ(0.21855796238512287,       increasing.GetResult(17, nullptr));
  EXPECT_DOUBLE_EQ(0.24920274817394159,       increasing.GetResult(18, nullptr));
  EXPECT_DOUBLE_EQ(0.037037037037037035,      increasing.GetResult(19, nullptr));
  EXPECT_DOUBLE_EQ(0.037037037037037035,      increasing.GetResult(20, nullptr)); // At model->max_age()
  EXPECT_DOUBLE_EQ(0.0,                       increasing.GetResult(21, nullptr)); // This is above model->max_age()
  EXPECT_DOUBLE_EQ(0.0,                       increasing.GetResult(22, nullptr));
  EXPECT_DOUBLE_EQ(0.0,                       increasing.GetResult(23, nullptr));
}

}

#endif /* ifdef TESTMODE */
