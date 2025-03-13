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
#include "CompoundAll.h"

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
TEST(Selectivities, CompoundAll_Age) {
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  EXPECT_CALL(*model, min_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(*model, max_age()).WillRepeatedly(Return(20));
  EXPECT_CALL(*model, age_spread()).WillRepeatedly(Return(11));
  EXPECT_CALL(*model, partition_type()).WillRepeatedly(Return(PartitionType::kAge));

  niwa::selectivities::CompoundAll selectivity(model);

  selectivity.parameters().Add(PARAM_LABEL, "unit_test_selectivity", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_A50, "8", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_ATO95, "3", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_A_MIN, "0.1", __FILE__, __LINE__);
  selectivity.Validate();
  selectivity.Build();

  // Math: (1.0 - amin_) / (1.0 + pow(19.0, (a50_ - value) / a_to95_)) + amin_;
  EXPECT_DOUBLE_EQ(0.8891675757731119, selectivity.GetAgeResult(10, nullptr));  // At model->min_age()
  EXPECT_DOUBLE_EQ(0.95500000000000007, selectivity.GetAgeResult(11, nullptr));
  EXPECT_DOUBLE_EQ(0.98259175207014837, selectivity.GetAgeResult(12, nullptr));
  EXPECT_DOUBLE_EQ(0.99339628773640465, selectivity.GetAgeResult(13, nullptr));
  EXPECT_DOUBLE_EQ(0.99751381215469614, selectivity.GetAgeResult(14, nullptr));
  EXPECT_DOUBLE_EQ(0.99906667371380831, selectivity.GetAgeResult(15, nullptr));
  EXPECT_DOUBLE_EQ(0.99965000327277953, selectivity.GetAgeResult(16, nullptr));
  EXPECT_DOUBLE_EQ(0.99986880466472305, selectivity.GetAgeResult(17, nullptr));
  EXPECT_DOUBLE_EQ(0.99995082925615009, selectivity.GetAgeResult(18, nullptr));
  EXPECT_DOUBLE_EQ(0.99998157233054197, selectivity.GetAgeResult(19, nullptr));
  EXPECT_DOUBLE_EQ(0.99999309402863679, selectivity.GetAgeResult(20, nullptr));  // At model->max_age()
}

/**
 * Test the values of this selectivity when using a Length based model
 */
TEST(Selectivities, CompoundAll_Length) {
  shared_ptr<MockModelLength> model            = shared_ptr<MockModelLength>(new MockModelLength());
  vector<double>              lengths          = {10, 20, 30, 40, 50, 60, 120};
  vector<double>              length_midpoints = {15, 25, 35, 45, 55, 65, 130};

  EXPECT_CALL(*model, partition_type()).WillRepeatedly(Return(PartitionType::kLength));
  model->set_length_bins(lengths);
  model->set_length_plus(true);
  model->set_length_midpoints(length_midpoints);
  model->set_number_of_length_bins();  // if we chnage plus group need to reset thsi
  model->bind_calls();

  niwa::selectivities::CompoundAll selectivity(model);

  vector<double> values = {0.99906667371380831, 0.99999994895278188, 0.99999999999721079, 0.99999999999999978, 1.0, 1.0, 1.0};

  selectivity.parameters().Add(PARAM_LABEL, "unit_test_selectivity", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_A50, "8", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_ATO95, "3", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_A_MIN, "0.1", __FILE__, __LINE__);
  selectivity.Validate();
  selectivity.Build();

  for (unsigned i = 0; i < values.size(); ++i) {
    EXPECT_DOUBLE_EQ(values[i], selectivity.GetLengthResult(i))
        << "i = " << i << " value " << values[i] << " midpoint = " << length_midpoints[i] << " result = " << selectivity.GetLengthResult(i);
  }
}

}  // namespace niwa

#endif /* ifdef TESTMODE */
