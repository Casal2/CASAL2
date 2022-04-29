/**
 * @file AllValues.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifdef TESTMODE

// Headers
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/lexical_cast.hpp>
#include <string>

#include "../../TestResources/MockClasses/Model.h"
#include "../../TestResources/MockClasses/ModelLength.h"

#include "../../Utilities/PartitionType.h"
#include "AllValues.h"

// Namespaces
namespace niwa {

using ::testing::Return;
using ::testing::ReturnRef;

/**
 * Test the values of this selectivity when using an Age based model
 */
TEST(Selectivities, AllValues_Age) {
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  EXPECT_CALL(*model, min_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(*model, max_age()).WillRepeatedly(Return(20));
  EXPECT_CALL(*model, age_spread()).WillRepeatedly(Return(11));
  EXPECT_CALL(*model, partition_type()).WillRepeatedly(Return(PartitionType::kAge));

  niwa::selectivities::AllValues all_values(model);

  vector<string> v;
  for (unsigned i = 0; i < 11; ++i) v.push_back(boost::lexical_cast<string>(i + 1));

  all_values.parameters().Add(PARAM_LABEL, "unit_test_all_values", __FILE__, __LINE__);
  all_values.parameters().Add(PARAM_V, v, __FILE__, __LINE__);
  all_values.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  all_values.Validate();
  all_values.Build();

  //ASSERT_THROW(all_values.GetAgeResult(9, nullptr), std::string);  // Below model->min_age()
  EXPECT_DOUBLE_EQ(1.0, all_values.GetAgeResult(10, nullptr));     // At model->min_age()
  EXPECT_DOUBLE_EQ(2.0, all_values.GetAgeResult(11, nullptr));
  EXPECT_DOUBLE_EQ(3.0, all_values.GetAgeResult(12, nullptr));
  EXPECT_DOUBLE_EQ(4.0, all_values.GetAgeResult(13, nullptr));
  EXPECT_DOUBLE_EQ(5.0, all_values.GetAgeResult(14, nullptr));
  EXPECT_DOUBLE_EQ(6.0, all_values.GetAgeResult(15, nullptr));
  EXPECT_DOUBLE_EQ(7.0, all_values.GetAgeResult(16, nullptr));
  EXPECT_DOUBLE_EQ(8.0, all_values.GetAgeResult(17, nullptr));
  EXPECT_DOUBLE_EQ(9.0, all_values.GetAgeResult(18, nullptr));
  EXPECT_DOUBLE_EQ(10.0, all_values.GetAgeResult(19, nullptr));
  EXPECT_DOUBLE_EQ(11.0, all_values.GetAgeResult(20, nullptr));     // At model->max_age()
  //ASSERT_THROW(all_values.GetAgeResult(21, nullptr), std::string);  // This is above model->max_age()
}

/**
 * Test the values of this selectivity when using a Length based model
 */
TEST(Selectivities, AllValues_Length) {
  shared_ptr<MockModelLength> model   = shared_ptr<MockModelLength>(new MockModelLength());
  vector<double>        lengths = {10, 20, 30, 40, 50, 60, 120};
  vector<double>        length_midpoints = {15, 25, 35, 45, 55, 65, 130};

  EXPECT_CALL(*model, partition_type()).WillRepeatedly(Return(PartitionType::kLength));
  model->set_length_bins(lengths);
  model->set_length_plus(true);
  model->set_length_midpoints(length_midpoints);
  model->set_number_of_length_bins(); // if we chnage plus group need to reset thsi
  model->bind_calls();

  niwa::selectivities::AllValues all_values(model);

  vector<string> v      = {"0.0", "0.1", "0.2", "0.3", "0.1", "0.5", "0.0325"};
  vector<double> values = {0.0, 0.1, 0.2, 0.3, 0.1, 0.5, 0.0325};

  all_values.parameters().Add(PARAM_LABEL, "unit_test_all_values", __FILE__, __LINE__);
  all_values.parameters().Add(PARAM_V, v, __FILE__, __LINE__);
  all_values.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  all_values.Validate();
  all_values.Build();

  for (unsigned i = 0; i < v.size(); ++i) {
    EXPECT_DOUBLE_EQ(values[i], all_values.GetLengthResult(i)) << "i = " << i << " value " << values[i] << " midpoint = " << length_midpoints[i] << " result = " << all_values.GetLengthResult(i);
  }
}

}  // namespace niwa

#endif /* ifdef TESTMODE */
