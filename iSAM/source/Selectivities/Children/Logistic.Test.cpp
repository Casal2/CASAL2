/**
 * @file InverseLogistic.Test.cpp
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
#include "Logistic.h"

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
TEST(Selectivities, Logistic) {
  MockModel model;
  EXPECT_CALL(model, min_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(model, max_age()).WillRepeatedly(Return(20));

  niwa::selectivities::Logistic logistic(&model);

  logistic.parameters().Add(PARAM_LABEL, "unit_test_logistic", __FILE__, __LINE__);
  logistic.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  logistic.parameters().Add(PARAM_A50,   "2",  __FILE__, __LINE__);
  logistic.parameters().Add(PARAM_ATO95, "7",  __FILE__, __LINE__);
  logistic.Validate();
  logistic.Build();

  EXPECT_DOUBLE_EQ(0.0,                       logistic.GetResult(9)); // Below model->min_age()
  EXPECT_DOUBLE_EQ(0.96659497164362229,       logistic.GetResult(10)); // At model->min_age()
  EXPECT_DOUBLE_EQ(0.97781072943439207,       logistic.GetResult(11));
  EXPECT_DOUBLE_EQ(0.98531798872758125,       logistic.GetResult(12));
  EXPECT_DOUBLE_EQ(0.99031049840094476,       logistic.GetResult(13));
  EXPECT_DOUBLE_EQ(0.99361634077929817,       logistic.GetResult(14));
  EXPECT_DOUBLE_EQ(0.99579908776852011,       logistic.GetResult(15));
  EXPECT_DOUBLE_EQ(0.99723756906077354,       logistic.GetResult(16));
  EXPECT_DOUBLE_EQ(0.99818438198748194,       logistic.GetResult(17));
  EXPECT_DOUBLE_EQ(0.99880706650531281,       logistic.GetResult(18));
  EXPECT_DOUBLE_EQ(0.99921636273936254,       logistic.GetResult(19));
  EXPECT_DOUBLE_EQ(0.99948530154281656,       logistic.GetResult(20)); // At model->max_age()
  EXPECT_DOUBLE_EQ(0.0,                       logistic.GetResult(21)); // This is above model->max_age()
  EXPECT_DOUBLE_EQ(0.0,                       logistic.GetResult(22));
  EXPECT_DOUBLE_EQ(0.0,                       logistic.GetResult(23));
}

}

#endif /* ifdef TESTMODE */
