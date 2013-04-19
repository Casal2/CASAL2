/**
 * @file LogisticProducting.Test.cpp
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
#include "LogisticProducing.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <boost/lexical_cast.hpp>

#include "TestResources/MockClasses/Model.h"

// Namespaces
namespace isam {

using ::testing::Return;

/**
 * Test the results of our selectivity are correct
 */
TEST(Selectivities, LogisticProducing) {
  boost::shared_ptr<MockModel> model = boost::shared_ptr<MockModel>(new MockModel);
  EXPECT_CALL(*model.get(), min_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(*model.get(), max_age()).WillRepeatedly(Return(20));

  isam::selectivities::LogisticProducing logistic_producing(model);

  logistic_producing.parameters().Add(PARAM_LABEL, "unit_test_logistic_producing", __FILE__, __LINE__);
  logistic_producing.parameters().Add(PARAM_L,     "13",  __FILE__, __LINE__);
  logistic_producing.parameters().Add(PARAM_H,     "17",  __FILE__, __LINE__);
  logistic_producing.parameters().Add(PARAM_A50,   "2",  __FILE__, __LINE__);
  logistic_producing.parameters().Add(PARAM_ATO95, "7",  __FILE__, __LINE__);
  logistic_producing.Validate();
  logistic_producing.Build();

  EXPECT_DOUBLE_EQ(0.0,                       logistic_producing.GetResult(9)); // Below model->min_age()
  EXPECT_DOUBLE_EQ(0.0,                       logistic_producing.GetResult(10)); // At model->min_age()
  EXPECT_DOUBLE_EQ(0.0,                       logistic_producing.GetResult(11));
  EXPECT_DOUBLE_EQ(0.0,                       logistic_producing.GetResult(12));
  EXPECT_DOUBLE_EQ(0.99031049840094476,       logistic_producing.GetResult(13));
  EXPECT_DOUBLE_EQ(0.3411777524940745,        logistic_producing.GetResult(14));
  EXPECT_DOUBLE_EQ(0.34192724168975369,       logistic_producing.GetResult(15));
  EXPECT_DOUBLE_EQ(0.34242117259057536,       logistic_producing.GetResult(16));
  EXPECT_DOUBLE_EQ(1.0,                       logistic_producing.GetResult(17));
  EXPECT_DOUBLE_EQ(1.0,                       logistic_producing.GetResult(18));
  EXPECT_DOUBLE_EQ(1.0,                       logistic_producing.GetResult(19));
  EXPECT_DOUBLE_EQ(1.0,                       logistic_producing.GetResult(20)); // At model->max_age()
  EXPECT_DOUBLE_EQ(0.0,                       logistic_producing.GetResult(21)); // This is above model->max_age()
  EXPECT_DOUBLE_EQ(0.0,                       logistic_producing.GetResult(22));
  EXPECT_DOUBLE_EQ(0.0,                       logistic_producing.GetResult(23));
}

}

#endif /* ifdef TESTMODE */
