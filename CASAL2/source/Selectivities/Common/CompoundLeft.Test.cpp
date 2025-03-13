/**
 * @file CompoundAll.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2025/03/13
 * @section LICENSE
 *
 * Copyright Casal2 Project 2025 - https://github.com/Casal2/
 *
 */
#ifdef TESTMODE

// Headers
#include "CompoundLeft.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/lexical_cast.hpp>
#include <string>

#include "../../TestResources/MockClasses/Model.h"
#include "../../TestResources/MockClasses/ModelLength.h"
#include "../../Utilities/PartitionType.h"

// Namespaces
namespace niwa {

using ::testing::Return;
using ::testing::ReturnRef;

/**
 * Test the values of this selectivity when using an Age based model
 */
TEST(Selectivities, CompoundLeft_Age) {
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  EXPECT_CALL(*model, min_age()).WillRepeatedly(Return(5));
  EXPECT_CALL(*model, max_age()).WillRepeatedly(Return(20));
  EXPECT_CALL(*model, age_spread()).WillRepeatedly(Return(16));
  EXPECT_CALL(*model, partition_type()).WillRepeatedly(Return(PartitionType::kAge));

  niwa::selectivities::CompoundLeft selectivity(model);

  selectivity.parameters().Add(PARAM_LABEL, "unit_test_selectivity", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_A50, "5", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_ATO95, "3", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_A_MIN, "0.05", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_LEFT_MEAN, "12", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_SIGMA, "3", __FILE__, __LINE__);
  selectivity.Validate();
  selectivity.Build();

  // Math: (1.0 - amin_) / (1.0 + pow(19.0, (a50_ - value) / a_to95_)) + amin_;
  EXPECT_DOUBLE_EQ(0.52445555966638824, selectivity.get_value(5u));
  EXPECT_DOUBLE_EQ(0.52445555966638824, selectivity.GetAgeResult(5, nullptr));

  EXPECT_DOUBLE_EQ(0.73898458006819778, selectivity.GetAgeResult(6, nullptr));
  EXPECT_DOUBLE_EQ(0.87653116841468204, selectivity.GetAgeResult(7, nullptr));
  EXPECT_DOUBLE_EQ(0.93407627094090706, selectivity.GetAgeResult(8, nullptr));
  EXPECT_DOUBLE_EQ(0.93254339582589874, selectivity.GetAgeResult(9, nullptr));
  EXPECT_DOUBLE_EQ(0.87074068441672126, selectivity.GetAgeResult(10, nullptr));
  EXPECT_DOUBLE_EQ(0.72549278733970901, selectivity.GetAgeResult(11, nullptr));
  EXPECT_DOUBLE_EQ(0.49950741112673214, selectivity.GetAgeResult(12, nullptr));
  EXPECT_DOUBLE_EQ(0.27249757652857426, selectivity.GetAgeResult(13, nullptr));
  EXPECT_DOUBLE_EQ(0.12313008412592108, selectivity.GetAgeResult(14, nullptr));
  EXPECT_DOUBLE_EQ(0.04999740487740785, selectivity.GetAgeResult(15, nullptr));
  EXPECT_DOUBLE_EQ(0.019342121460616871, selectivity.GetAgeResult(16, nullptr));
  EXPECT_DOUBLE_EQ(0.0073374045832597188, selectivity.GetAgeResult(17, nullptr));
  EXPECT_DOUBLE_EQ(0.0027624233926811848, selectivity.GetAgeResult(18, nullptr));
  EXPECT_DOUBLE_EQ(0.0010370281451906396, selectivity.GetAgeResult(19, nullptr));
  EXPECT_DOUBLE_EQ(0.00038888510326438928, selectivity.GetAgeResult(20, nullptr));
}

/**
 * Test the values of this selectivity when using a Length based model
 */
TEST(Selectivities, CompoundLeft_Length) {
  shared_ptr<MockModelLength> model            = shared_ptr<MockModelLength>(new MockModelLength());
  vector<double>              lengths          = {10, 20, 30, 40, 50, 60, 120};
  vector<double>              length_midpoints = {15, 25, 35, 45, 55, 65, 130};

  EXPECT_CALL(*model, partition_type()).WillRepeatedly(Return(PartitionType::kLength));
  model->set_length_bins(lengths);
  model->set_length_plus(true);
  model->set_length_midpoints(length_midpoints);
  model->set_number_of_length_bins();  // if we chnage plus group need to reset thsi
  model->bind_calls();

  niwa::selectivities::CompoundLeft selectivity(model);

  vector<double> values = {0.04999740487740785, 2.8756309271066102e-06, 1.5711676404348348e-10, 8.659739592076221e-15, 0.0, 0.0, 0.0};

  selectivity.parameters().Add(PARAM_LABEL, "unit_test_selectivity", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_A50, "5", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_ATO95, "3", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_A_MIN, "0.05", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_LEFT_MEAN, "12", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_SIGMA, "3", __FILE__, __LINE__);
  selectivity.Validate();
  selectivity.Build();

  for (unsigned i = 0; i < values.size(); ++i) {
    EXPECT_DOUBLE_EQ(values[i], selectivity.GetLengthResult(i))
        << "i = " << i << " value " << values[i] << " midpoint = " << length_midpoints[i] << " result = " << selectivity.GetLengthResult(i);
  }
}

}  // namespace niwa

#endif /* ifdef TESTMODE */
