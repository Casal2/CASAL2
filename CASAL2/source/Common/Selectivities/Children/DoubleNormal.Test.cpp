/**
 * @file DoubleNormal.Test.cpp
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
#include "DoubleNormal.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <boost/lexical_cast.hpp>
#include <string>

#include "TestResources/MockClasses/Model.h"

// Namespaces
namespace niwa {

using ::testing::Return;
using ::testing::ReturnRef;

/**
 * Test the results of our selectivity are correct
 */
TEST(Selectivities, DoubleNormal_Age) {
  MockModel model;
  EXPECT_CALL(model, min_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(model, max_age()).WillRepeatedly(Return(20));
  EXPECT_CALL(model, age_spread()).WillRepeatedly(Return(11));
  EXPECT_CALL(model, partition_type()).WillRepeatedly(Return(PartitionType::kAge));

  niwa::selectivities::DoubleNormal double_normal(&model);

  double_normal.parameters().Add(PARAM_LABEL, "unit_test_double_normal", __FILE__, __LINE__);
  double_normal.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  double_normal.parameters().Add(PARAM_MU, "15",  __FILE__, __LINE__);
  double_normal.parameters().Add(PARAM_SIGMA_L, "1",  __FILE__, __LINE__);
  double_normal.parameters().Add(PARAM_SIGMA_R, "10",  __FILE__, __LINE__);
  double_normal.Validate();
  double_normal.Build();

  ASSERT_THROW(double_normal.GetAgeResult(9, nullptr), std::string); // Below model->min_age()
  EXPECT_DOUBLE_EQ(2.9802322387695312e-008,   double_normal.GetAgeResult(10, nullptr)); // At model->min_age()
  EXPECT_DOUBLE_EQ(1.52587890625e-005,        double_normal.GetAgeResult(11, nullptr));
  EXPECT_DOUBLE_EQ(0.001953125,               double_normal.GetAgeResult(12, nullptr));
  EXPECT_DOUBLE_EQ(0.0625,                    double_normal.GetAgeResult(13, nullptr));
  EXPECT_DOUBLE_EQ(0.5,                       double_normal.GetAgeResult(14, nullptr));
  EXPECT_DOUBLE_EQ(1.0,                       double_normal.GetAgeResult(15, nullptr));
  EXPECT_DOUBLE_EQ(0.99309249543703593,       double_normal.GetAgeResult(16, nullptr));
  EXPECT_DOUBLE_EQ(0.97265494741228553,       double_normal.GetAgeResult(17, nullptr));
  EXPECT_DOUBLE_EQ(0.9395227492140118,        double_normal.GetAgeResult(18, nullptr));
  EXPECT_DOUBLE_EQ(0.89502507092797245,       double_normal.GetAgeResult(19, nullptr));
  EXPECT_DOUBLE_EQ(0.8408964152537145,        double_normal.GetAgeResult(20, nullptr)); // At model->max_age()
  ASSERT_THROW(double_normal.GetAgeResult(21, nullptr), std::string); // This is above model->max_age()
}

TEST(Selectivities, DoubleNormal_Length) {
  MockModel model;
  vector<unsigned> lengths = {10, 20, 30, 40, 50, 60};
  EXPECT_CALL(model, min_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(model, max_age()).WillRepeatedly(Return(20));
  EXPECT_CALL(model, age_spread()).WillRepeatedly(Return(11));
  EXPECT_CALL(model, length_bins()).WillRepeatedly(ReturnRef(lengths));
  EXPECT_CALL(model, partition_type()).WillRepeatedly(Return(PartitionType::kLength));

  niwa::selectivities::DoubleNormal double_normal(&model);

  vector<double> expected_values = {2.980232238769531e-08, 8.408964152537145e-01, 2.102241038134286e-01, 1.313900648833929e-02, 2.052969763803014e-04, 8.019413139855523e-07};

  double_normal.parameters().Add(PARAM_LABEL, "unit_test_double_normal", __FILE__, __LINE__);
  double_normal.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  double_normal.parameters().Add(PARAM_MU, "15",  __FILE__, __LINE__);
  double_normal.parameters().Add(PARAM_SIGMA_L, "1",  __FILE__, __LINE__);
  double_normal.parameters().Add(PARAM_SIGMA_R, "10",  __FILE__, __LINE__);
  double_normal.Validate();
  double_normal.Build();

  for (unsigned i = 0; i < lengths.size(); ++i) {
    EXPECT_DOUBLE_EQ(expected_values[i],  double_normal.GetLengthResult(i));
  }

}

}

#endif /* ifdef TESTMODE */
