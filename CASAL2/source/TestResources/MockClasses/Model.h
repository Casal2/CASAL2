/**
 * @file Model.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef MOCK_MODEL_H_
#define MOCK_MODEL_H_

// Headers
#include <gmock/gmock.h>

#include "Common/Model/Model.h"
#include "Common/Model/Managers.h"
#include "Common/Utilities/PartitionType.h"

// Namespaces
namespace niwa {
using ::testing::Return;
using ::testing::ReturnRef;

/**
 * Class Definition
 */
class MockModel : public niwa::Model {
public:
  MOCK_CONST_METHOD0(min_age, unsigned());
  MOCK_CONST_METHOD0(max_age, unsigned());
  MOCK_CONST_METHOD0(age_spread, unsigned());
  MOCK_CONST_METHOD0(start_year, unsigned());
  MOCK_CONST_METHOD0(final_year, unsigned());
  MOCK_CONST_METHOD0(age_plus, bool());
  MOCK_CONST_METHOD0(current_year, unsigned());
  MOCK_CONST_METHOD0(time_steps, vector<string>&());
  MOCK_CONST_METHOD0(years, vector<unsigned>());
  MOCK_CONST_METHOD0(partition_type, PartitionType());
  MOCK_CONST_METHOD0(length_bins, vector<unsigned>&());
  MOCK_CONST_METHOD0(length_plus, bool());
  MOCK_METHOD0(managers, niwa::Managers&());
  MOCK_METHOD0(categories, niwa::Categories*());

  vector<string> mock_time_steps_ = {"time_step_one", "time_step_two"};
  vector<unsigned> mock_years_ = { 1990, 1991, 1992 };
  vector<unsigned> mock_length_bins_ = { 10, 20, 30, 40, 50 };
  bool mock_length_plus_ = false;
};


inline void init_mock_model_01(MockModel& model) {
  EXPECT_CALL(model, min_age()).WillRepeatedly(Return(1));
  EXPECT_CALL(model, max_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(model, age_spread()).WillRepeatedly(Return(10));
  EXPECT_CALL(model, start_year()).WillRepeatedly(Return(1990));
  EXPECT_CALL(model, final_year()).WillRepeatedly(Return(1992));
  EXPECT_CALL(model, current_year()).WillRepeatedly(Return(1991));
  EXPECT_CALL(model, age_plus()).WillRepeatedly(Return(true));
  EXPECT_CALL(model, time_steps()).WillRepeatedly(ReturnRef(model.mock_time_steps_));
  EXPECT_CALL(model, years()).WillRepeatedly(Return(model.mock_years_));
  EXPECT_CALL(model, partition_type()).WillRepeatedly(Return(PartitionType::kAge));
  EXPECT_CALL(model, length_bins()).WillRepeatedly(ReturnRef(model.mock_length_bins_));
  EXPECT_CALL(model, length_plus()).WillRepeatedly(Return(model.mock_length_plus_));
}

} /* namespace niwa */

#endif /* MOCK_MODEL_H_ */
