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
#include "Common/TimeSteps/Manager.h"
#include "Age/AgeLengths/AgeLength.h"
#include "Age/AgeLengths/Manager.h"

// Namespaces
namespace niwa {

using ::testing::Return;
using ::testing::ReturnRef;

/**
 * Class Definition
 */
class MockAgeLength : public niwa::AgeLength {
public:
  MOCK_CONST_METHOD0(distribution, string());
  MOCK_METHOD2(mean_length, Double(unsigned year, unsigned age));
  MOCK_METHOD2(mean_weight, Double(unsigned year, unsigned age));
  MOCK_METHOD3(cv, Double(unsigned year, unsigned age, unsigned time_step));
};



class MockTimeStepManager : public timesteps::Manager {
public:
  unsigned time_step_index_ = 0;
  unsigned current_time_step() const override final { return time_step_index_; }
};

/**
 * Test the results of our selectivity are correct
 */
TEST(Selectivities, Logistic_Age) {
  MockModel model;
  EXPECT_CALL(model, min_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(model, max_age()).WillRepeatedly(Return(20));
  EXPECT_CALL(model, partition_type()).WillRepeatedly(Return(PartitionType::kAge));

  niwa::selectivities::Logistic logistic(&model);

  logistic.parameters().Add(PARAM_LABEL, "unit_test_logistic", __FILE__, __LINE__);
  logistic.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  logistic.parameters().Add(PARAM_A50,   "2",  __FILE__, __LINE__);
  logistic.parameters().Add(PARAM_ATO95, "7",  __FILE__, __LINE__);
  logistic.Validate();
  logistic.Build();

  EXPECT_DOUBLE_EQ(0.0,                       logistic.GetAgeResult(9, nullptr)); // Below model->min_age()
  EXPECT_DOUBLE_EQ(0.96659497164362229,       logistic.GetAgeResult(10, nullptr)); // At model->min_age()
  EXPECT_DOUBLE_EQ(0.97781072943439207,       logistic.GetAgeResult(11, nullptr));
  EXPECT_DOUBLE_EQ(0.98531798872758125,       logistic.GetAgeResult(12, nullptr));
  EXPECT_DOUBLE_EQ(0.99031049840094476,       logistic.GetAgeResult(13, nullptr));
  EXPECT_DOUBLE_EQ(0.99361634077929817,       logistic.GetAgeResult(14, nullptr));
  EXPECT_DOUBLE_EQ(0.99579908776852011,       logistic.GetAgeResult(15, nullptr));
  EXPECT_DOUBLE_EQ(0.99723756906077354,       logistic.GetAgeResult(16, nullptr));
  EXPECT_DOUBLE_EQ(0.99818438198748194,       logistic.GetAgeResult(17, nullptr));
  EXPECT_DOUBLE_EQ(0.99880706650531281,       logistic.GetAgeResult(18, nullptr));
  EXPECT_DOUBLE_EQ(0.99921636273936254,       logistic.GetAgeResult(19, nullptr));
  EXPECT_DOUBLE_EQ(0.99948530154281656,       logistic.GetAgeResult(20, nullptr)); // At model->max_age()
  EXPECT_DOUBLE_EQ(0.0,                       logistic.GetAgeResult(21, nullptr)); // This is above model->max_age()
  EXPECT_DOUBLE_EQ(0.0,                       logistic.GetAgeResult(22, nullptr));
  EXPECT_DOUBLE_EQ(0.0,                       logistic.GetAgeResult(23, nullptr));
}

TEST(Selectivities, Logistic_Length) {
  MockModel model;

  vector<unsigned> lengths = {10, 20, 30, 40, 50, 60, 120};
  EXPECT_CALL(model, length_bins()).WillRepeatedly(ReturnRef(lengths));
  EXPECT_CALL(model, partition_type()).WillRepeatedly(Return(PartitionType::kLength));

  niwa::selectivities::Logistic logistic(&model);

  vector<double> expected_values = {0.6431099137425979606, 0.5000000000000000000, 0.3568900862574020394, 0.2354523864622996010, 0.1459577602048917067, 0.0866255450880920597, 0.0027624309392264568};

  logistic.parameters().Add(PARAM_LABEL, "unit_test_logistic", __FILE__, __LINE__);
  logistic.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  logistic.parameters().Add(PARAM_A50,   "20",  __FILE__, __LINE__);
  logistic.parameters().Add(PARAM_ATO95, "50",  __FILE__, __LINE__);
  logistic.Validate();
  logistic.Build();

  for (unsigned i = 0; i < lengths.size(); ++i) {
    EXPECT_DOUBLE_EQ(expected_values[i],  logistic.GetLengthResult(i));
  }

}

}

#endif /* ifdef TESTMODE */
