/**
 * @file DoubleExponential.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 22/01/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifdef TESTMODE

// Headers
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/lexical_cast.hpp>

#include "../../TestResources/MockClasses/Model.h"
#include "../../TestResources/MockClasses/ModelLength.h"

#include "DoubleExponential.h"

// Namespaces
namespace niwa {

using ::testing::Return;
using ::testing::ReturnRef;

/**
 * Test the results of our selectivity are correct
 */
TEST(Selectivities, DoubleExponential_Age) {
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  EXPECT_CALL(*model, min_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(*model, max_age()).WillRepeatedly(Return(20));
  EXPECT_CALL(*model, age_spread()).WillRepeatedly(Return(11));
  EXPECT_CALL(*model, partition_type()).WillRepeatedly(Return(PartitionType::kAge));

  niwa::selectivities::DoubleExponential double_exponential(model);

  double_exponential.parameters().Add(PARAM_LABEL, "unit_test_double_exponential", __FILE__, __LINE__);
  double_exponential.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  double_exponential.parameters().Add(PARAM_X0, "8", __FILE__, __LINE__);
  double_exponential.parameters().Add(PARAM_X1, "1", __FILE__, __LINE__);
  double_exponential.parameters().Add(PARAM_X2, "23", __FILE__, __LINE__);
  double_exponential.parameters().Add(PARAM_Y0, "0.1", __FILE__, __LINE__);
  double_exponential.parameters().Add(PARAM_Y1, "1", __FILE__, __LINE__);
  double_exponential.parameters().Add(PARAM_Y2, "1", __FILE__, __LINE__);
  double_exponential.Validate();
  double_exponential.Build();

  ASSERT_THROW(double_exponential.GetAgeResult(9, nullptr), std::string);               // Below model->min_age()
  EXPECT_DOUBLE_EQ(0.13593563908785256, double_exponential.GetAgeResult(10, nullptr));  // At model->min_age()
  EXPECT_DOUBLE_EQ(0.15848931924611137, double_exponential.GetAgeResult(11, nullptr));
  EXPECT_DOUBLE_EQ(0.1847849797422291, double_exponential.GetAgeResult(12, nullptr));
  EXPECT_DOUBLE_EQ(0.21544346900318839, double_exponential.GetAgeResult(13, nullptr));
  EXPECT_DOUBLE_EQ(0.25118864315095801, double_exponential.GetAgeResult(14, nullptr));
  EXPECT_DOUBLE_EQ(0.29286445646252368, double_exponential.GetAgeResult(15, nullptr));
  EXPECT_DOUBLE_EQ(0.34145488738336016, double_exponential.GetAgeResult(16, nullptr));
  EXPECT_DOUBLE_EQ(0.39810717055349726, double_exponential.GetAgeResult(17, nullptr));
  EXPECT_DOUBLE_EQ(0.46415888336127786, double_exponential.GetAgeResult(18, nullptr));
  EXPECT_DOUBLE_EQ(0.54116952654646366, double_exponential.GetAgeResult(19, nullptr));
  EXPECT_DOUBLE_EQ(0.63095734448019336, double_exponential.GetAgeResult(20, nullptr));  // At model->max_age()
  ASSERT_THROW(double_exponential.GetAgeResult(21, nullptr), std::string);              // This is above model->max_age()
}

TEST(Selectivities, DoubleExponential_Length) {
  shared_ptr<MockModelLength> model   = shared_ptr<MockModelLength>(new MockModelLength());
  vector<double>        lengths = {10, 12, 14, 16, 18, 20};
  EXPECT_CALL(*model, length_bin_mid_points()).WillRepeatedly(ReturnRef(lengths));
  EXPECT_CALL(*model, partition_type()).WillRepeatedly(Return(PartitionType::kLength));


  niwa::selectivities::DoubleExponential double_exponential(model);

  vector<double> expected_values = {0.13593563908785256, 0.1847849797422291, 0.25118864315095801, 0.34145488738336016, 0.46415888336127786, 0.63095734448019336};

  double_exponential.parameters().Add(PARAM_LABEL, "unit_test_double_exponential", __FILE__, __LINE__);
  double_exponential.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  double_exponential.parameters().Add(PARAM_X0, "8", __FILE__, __LINE__);
  double_exponential.parameters().Add(PARAM_X1, "1", __FILE__, __LINE__);
  double_exponential.parameters().Add(PARAM_X2, "23", __FILE__, __LINE__);
  double_exponential.parameters().Add(PARAM_Y0, "0.1", __FILE__, __LINE__);
  double_exponential.parameters().Add(PARAM_Y1, "1", __FILE__, __LINE__);
  double_exponential.parameters().Add(PARAM_Y2, "1", __FILE__, __LINE__);
  double_exponential.Validate();
  double_exponential.Build();

  for (unsigned i = 0; i < lengths.size(); ++i) {
    EXPECT_DOUBLE_EQ(expected_values[i], double_exponential.GetLengthResult(i));
  }
}

}  // namespace niwa

#endif /* ifdef TESTMODE */
