/**
 * @file CompoundMiddle.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2025/03/13
 * @section LICENSE
 *
 * Copyright Casal2 Project 2025 - https://github.com/Casal2/
 *
 */
#ifdef TESTMODE

// Headers
#include "CompoundMiddle.h"

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
TEST(Selectivities, CompoundMiddle_Age) {
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  EXPECT_CALL(*model, min_age()).WillRepeatedly(Return(5));
  EXPECT_CALL(*model, max_age()).WillRepeatedly(Return(20));
  EXPECT_CALL(*model, age_spread()).WillRepeatedly(Return(16));
  EXPECT_CALL(*model, partition_type()).WillRepeatedly(Return(PartitionType::kAge));

  niwa::selectivities::CompoundMiddle selectivity(model);

  selectivity.parameters().Add(PARAM_LABEL, "unit_test_selectivity", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_A50, "5", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_ATO95, "3", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_A_MIN, "0.05", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_LEFT_MEAN, "12", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_TO_RIGHT_MEAN, "2", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_SIGMA, "3", __FILE__, __LINE__);
  selectivity.Validate();
  selectivity.Build();

  // Math: (1.0 - amin_) / (1.0 + pow(19.0, (a50_ - value) / a_to95_)) + amin_;
  EXPECT_DOUBLE_EQ(0.00054436096913166557, selectivity.get_value(5u));
  EXPECT_DOUBLE_EQ(0.00054436096913166557, selectivity.GetAgeResult(5, nullptr));

  EXPECT_DOUBLE_EQ(0.0020462526312000238, selectivity.GetAgeResult(6, nullptr));
  EXPECT_DOUBLE_EQ(0.0064723314923793495, selectivity.GetAgeResult(7, nullptr));
  EXPECT_DOUBLE_EQ(0.018372834779924116, selectivity.GetAgeResult(8, nullptr));
  EXPECT_DOUBLE_EQ(0.048721099882102846, selectivity.GetAgeResult(9, nullptr));
  EXPECT_DOUBLE_EQ(0.11992336092936008, selectivity.GetAgeResult(10, nullptr));
  EXPECT_DOUBLE_EQ(0.25828875810462448, selectivity.GetAgeResult(11, nullptr));
  EXPECT_DOUBLE_EQ(0.43799450302176263, selectivity.GetAgeResult(12, nullptr));
  EXPECT_DOUBLE_EQ(0.52891777812005514, selectivity.GetAgeResult(13, nullptr));
  EXPECT_DOUBLE_EQ(0.43836571595453222, selectivity.GetAgeResult(14, nullptr));
  EXPECT_DOUBLE_EQ(0.25895493014013587, selectivity.GetAgeResult(15, nullptr));
  EXPECT_DOUBLE_EQ(0.12076281574077821, selectivity.GetAgeResult(16, nullptr));
  EXPECT_DOUBLE_EQ(0.049632765289015002, selectivity.GetAgeResult(17, nullptr));
  EXPECT_DOUBLE_EQ(0.019289012690925672, selectivity.GetAgeResult(18, nullptr));
  EXPECT_DOUBLE_EQ(0.0073298414081905171, selectivity.GetAgeResult(19, nullptr));
  EXPECT_DOUBLE_EQ(0.0027613556111294149, selectivity.GetAgeResult(20, nullptr));
}

/**
 * Test the values of this selectivity when using a Length based model
 */
TEST(Selectivities, CompoundMiddle_Length) {
  shared_ptr<MockModelLength> model            = shared_ptr<MockModelLength>(new MockModelLength());
  vector<double>              lengths          = {10, 20, 30, 40, 50, 60, 120};
  vector<double>              length_midpoints = {15, 25, 35, 45, 55, 65, 130};

  EXPECT_CALL(*model, partition_type()).WillRepeatedly(Return(PartitionType::kLength));
  model->set_length_bins(lengths);
  model->set_length_plus(true);
  model->set_length_midpoints(length_midpoints);
  model->set_number_of_length_bins();  // if we chnage plus group need to reset thsi
  model->bind_calls();

  niwa::selectivities::CompoundMiddle selectivity(model);

  vector<double> values = {0.25895493014013587, 2.0475129349476886e-05, 1.1187286650854704e-09, 6.106226635438308e-14, 0.0, 0.0, 0.0};

  selectivity.parameters().Add(PARAM_LABEL, "unit_test_selectivity", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_A50, "5", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_ATO95, "3", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_A_MIN, "0.05", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_LEFT_MEAN, "12", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_TO_RIGHT_MEAN, "2", __FILE__, __LINE__);
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
