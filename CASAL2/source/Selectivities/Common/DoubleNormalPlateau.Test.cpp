/**
 * @file DoubleNormalPlateau.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2025/03/13
 * @section LICENSE
 *
 * Copyright Casal2 Project 2025 - https://github.com/Casal2/
 *
 */
#ifdef TESTMODE

// Headers
#include "DoubleNormalPlateau.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/lexical_cast.hpp>
#include <string>

#include "../../TestResources/MockClasses/Model.h"
#include "../../TestResources/MockClasses/ModelLength.h"

// Namespaces
namespace niwa {

using ::testing::Return;
using ::testing::ReturnRef;

/**
 * Test the results of our selectivity are correct
 */
TEST(Selectivities, DoubleNormalPlateau_Age) {
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  EXPECT_CALL(*model, min_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(*model, max_age()).WillRepeatedly(Return(20));
  EXPECT_CALL(*model, age_spread()).WillRepeatedly(Return(11));
  EXPECT_CALL(*model, partition_type()).WillRepeatedly(Return(PartitionType::kAge));

  niwa::selectivities::DoubleNormalPlateau double_normal(model);

  double_normal.parameters().Add(PARAM_LABEL, "unit_test_double_normal", __FILE__, __LINE__);
  double_normal.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  double_normal.parameters().Add(PARAM_MU, "15", __FILE__, __LINE__);
  double_normal.parameters().Add(PARAM_SIGMA_L, "1", __FILE__, __LINE__);
  double_normal.parameters().Add(PARAM_SIGMA_R, "10", __FILE__, __LINE__);
  double_normal.parameters().Add(PARAM_A1, "14", __FILE__, __LINE__);
  double_normal.parameters().Add(PARAM_A2, "2", __FILE__, __LINE__);
  double_normal.parameters().Add(PARAM_ALPHA, "1.0", __FILE__, __LINE__);
  double_normal.parameters().Add(PARAM_BETA, "12", __FILE__, __LINE__);
  double_normal.Validate();
  double_normal.Build();

  EXPECT_DOUBLE_EQ(0, double_normal.GetAgeResult(10, nullptr));  // At model->min_age()
  EXPECT_DOUBLE_EQ(0, double_normal.GetAgeResult(11, nullptr));
  EXPECT_DOUBLE_EQ(0.0625, double_normal.GetAgeResult(12, nullptr));
  EXPECT_DOUBLE_EQ(0.5, double_normal.GetAgeResult(13, nullptr));
  EXPECT_DOUBLE_EQ(1.0, double_normal.GetAgeResult(14, nullptr));
  EXPECT_DOUBLE_EQ(1.0, double_normal.GetAgeResult(15, nullptr));
  EXPECT_DOUBLE_EQ(1.0, double_normal.GetAgeResult(16, nullptr));
  EXPECT_DOUBLE_EQ(0.99309249543703593, double_normal.GetAgeResult(17, nullptr));
  EXPECT_DOUBLE_EQ(0.97265494741228553, double_normal.GetAgeResult(18, nullptr));
  EXPECT_DOUBLE_EQ(0.9395227492140118, double_normal.GetAgeResult(19, nullptr));
  EXPECT_DOUBLE_EQ(0.89502507092797245, double_normal.GetAgeResult(20, nullptr));  // At model->max_age()
}

TEST(Selectivities, DoubleNormalPlateau_Length) {
  shared_ptr<MockModelLength> model   = shared_ptr<MockModelLength>(new MockModelLength());
  vector<double>              lengths = {10, 20, 30, 40, 50, 60};
  EXPECT_CALL(*model, length_bin_mid_points()).WillRepeatedly(ReturnRef(lengths));
  EXPECT_CALL(*model, partition_type()).WillRepeatedly(Return(PartitionType::kLength));

  niwa::selectivities::DoubleNormalPlateau double_normal(model);

  vector<double> expected_values = {0, 0.89502507092797245, 0.25702845666401669, 0.018453010334836414, 0.00033120222838077957, 1.4861384957513776e-06};

  double_normal.parameters().Add(PARAM_LABEL, "unit_test_double_normal", __FILE__, __LINE__);
  double_normal.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  double_normal.parameters().Add(PARAM_MU, "15", __FILE__, __LINE__);
  double_normal.parameters().Add(PARAM_SIGMA_L, "1", __FILE__, __LINE__);
  double_normal.parameters().Add(PARAM_SIGMA_R, "10", __FILE__, __LINE__);
  double_normal.parameters().Add(PARAM_A1, "14", __FILE__, __LINE__);
  double_normal.parameters().Add(PARAM_A2, "2", __FILE__, __LINE__);
  double_normal.parameters().Add(PARAM_ALPHA, "1.0", __FILE__, __LINE__);
  double_normal.parameters().Add(PARAM_BETA, "12", __FILE__, __LINE__);
  double_normal.Validate();
  double_normal.Build();

  for (unsigned i = 0; i < lengths.size(); ++i) {
    EXPECT_DOUBLE_EQ(expected_values[i], double_normal.GetLengthResult(i));
  }
}

}  // namespace niwa

#endif /* ifdef TESTMODE */
