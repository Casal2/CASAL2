/**
 * @file Basic.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 29/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "Basic.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <boost/lexical_cast.hpp>

#include "TestResources/MockClasses/Model.h"

// Namespaces
namespace niwa {

using ::testing::Return;

/**
 * Test the results of our KnifeEdge are correct
 */
TEST(LengthWeights, Basic) {
  // TODO: Add more unit tests
  niwa::lengthweights::Basic basic;

  basic.parameters().Add(PARAM_LABEL, "unit_test_basic", __FILE__, __LINE__);
  basic.parameters().Add(PARAM_TYPE, "any value is fine", __FILE__, __LINE__);
  basic.parameters().Add(PARAM_A, "1", __FILE__, __LINE__);
  basic.parameters().Add(PARAM_B, "1", __FILE__, __LINE__);
  basic.parameters().Add(PARAM_UNITS, "tonnes", __FILE__, __LINE__);

  basic.Validate();
  basic.Build();

  EXPECT_DOUBLE_EQ(1000.0, basic.mean_weight(1.0, "", 1.0));
  EXPECT_DOUBLE_EQ(2000.0, basic.mean_weight(2.0, "", 1.0));
  EXPECT_DOUBLE_EQ(3000.0, basic.mean_weight(3.0, "", 1.0));
  EXPECT_DOUBLE_EQ(4000.0, basic.mean_weight(4.0, "", 1.0));
  EXPECT_DOUBLE_EQ(5000.0, basic.mean_weight(5.0, "", 1.0));
}

TEST(LengthWeights, Basic2) {
  niwa::lengthweights::Basic basic;
  /**
   * Run some new parameters
   */
  basic.parameters().Clear();
  basic.parameters().Add(PARAM_LABEL, "unit_test_basic", __FILE__, __LINE__);
  basic.parameters().Add(PARAM_TYPE, "any value is fine", __FILE__, __LINE__);
  basic.parameters().Add(PARAM_A, "2", __FILE__, __LINE__);
  basic.parameters().Add(PARAM_B, "2", __FILE__, __LINE__);
  basic.parameters().Add(PARAM_UNITS, "tonnes", __FILE__, __LINE__);

  basic.Validate();
  basic.Build();

  EXPECT_DOUBLE_EQ(2000.0, basic.mean_weight(1.0, "", 1.0));
  EXPECT_DOUBLE_EQ(8000.0, basic.mean_weight(2.0, "", 1.0));
  EXPECT_DOUBLE_EQ(18000.0, basic.mean_weight(3.0, "", 1.0));
  EXPECT_DOUBLE_EQ(32000.0, basic.mean_weight(4.0, "", 1.0));
  EXPECT_DOUBLE_EQ(50000.0, basic.mean_weight(5.0, "", 1.0));
}

}


#endif
