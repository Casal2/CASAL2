/**
 * @file DoubleExponential.Test.cpp
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
#include "DoubleExponential.h"

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
TEST(Selectivities, DoubleExponential) {
  MockModel model;
  EXPECT_CALL(model, min_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(model, max_age()).WillRepeatedly(Return(20));

  niwa::selectivities::DoubleExponential double_exponential(&model);

  double_exponential.parameters().Add(PARAM_LABEL, "unit_test_double_exponential", __FILE__, __LINE__);
  double_exponential.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  double_exponential.parameters().Add(PARAM_X0, "8",  __FILE__, __LINE__);
  double_exponential.parameters().Add(PARAM_X1, "1", __FILE__, __LINE__);
  double_exponential.parameters().Add(PARAM_X2, "23", __FILE__, __LINE__);
  double_exponential.parameters().Add(PARAM_Y0, "0.1", __FILE__, __LINE__);
  double_exponential.parameters().Add(PARAM_Y1, "1", __FILE__, __LINE__);
  double_exponential.parameters().Add(PARAM_Y2, "1", __FILE__, __LINE__);
  double_exponential.Validate();
  double_exponential.Build();

  EXPECT_DOUBLE_EQ(0.0,                 double_exponential.GetResult(9, nullptr)); // Below model->min_age()
  EXPECT_DOUBLE_EQ(0.13593563908785256,  double_exponential.GetResult(10, nullptr)); // At model->min_age()
  EXPECT_DOUBLE_EQ(0.15848931924611137,  double_exponential.GetResult(11, nullptr));
  EXPECT_DOUBLE_EQ(0.1847849797422291,  double_exponential.GetResult(12, nullptr));
  EXPECT_DOUBLE_EQ(0.21544346900318839,   double_exponential.GetResult(13, nullptr));
  EXPECT_DOUBLE_EQ(0.25118864315095801,  double_exponential.GetResult(14, nullptr));
  EXPECT_DOUBLE_EQ(0.29286445646252368,  double_exponential.GetResult(15, nullptr));
  EXPECT_DOUBLE_EQ(0.34145488738336016,  double_exponential.GetResult(16, nullptr));
  EXPECT_DOUBLE_EQ(0.39810717055349726,  double_exponential.GetResult(17, nullptr));
  EXPECT_DOUBLE_EQ(0.46415888336127786,  double_exponential.GetResult(18, nullptr));
  EXPECT_DOUBLE_EQ(0.54116952654646366,  double_exponential.GetResult(19, nullptr));
  EXPECT_DOUBLE_EQ(0.63095734448019336,  double_exponential.GetResult(20, nullptr)); // At model->max_age()
  EXPECT_DOUBLE_EQ(0.0,                 double_exponential.GetResult(21, nullptr)); // This is above model->max_age()
  EXPECT_DOUBLE_EQ(0.0,                 double_exponential.GetResult(22, nullptr));
  EXPECT_DOUBLE_EQ(0.0,                 double_exponential.GetResult(23, nullptr));
}

}

#endif /* ifdef TESTMODE */
