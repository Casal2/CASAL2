/**
 * @file Model.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef MOCK_MODEL_H_
#define MOCK_MODEL_H_
#ifdef TESTMODE

// Headers
#include <gmock/gmock.h>

#include "../../Model/Managers.h"
#include "../../Model/Model.h"
#include "../../Model/Models/Age.h"
#include "../../Model/Objects.h"
#include "../../Utilities/PartitionType.h"


// Namespaces
namespace niwa {
using ::testing::DoDefault;
using ::testing::Return;
using ::testing::ReturnRef;

/**
 * Class Definition
 */
class MockModel : public niwa::model::Age, public std::enable_shared_from_this<MockModel> {
public:
  // Constructor
  MockModel() {
    set_min_age(1);
    set_max_age(10);
    set_start_year(1990);
    set_final_year(1992);
    set_current_year(1991);
    set_age_plus(true);
    set_time_steps({"time_step_one", "time_step_two"});
    set_partition_type(PartitionType::kAge);
    set_length_bins({10, 20, 30, 40, 50});
    set_length_plus(false);
  }

  virtual ~MockModel() = default;

  /**
   * Methods we're going to mock
   */
  MOCK_CONST_METHOD0(min_age, unsigned());
  MOCK_CONST_METHOD0(max_age, unsigned());
  MOCK_CONST_METHOD0(age_spread, unsigned());
  MOCK_CONST_METHOD0(start_year, unsigned());
  MOCK_CONST_METHOD0(final_year, unsigned());
  MOCK_CONST_METHOD0(projection_final_year, unsigned());
  MOCK_CONST_METHOD0(age_plus, bool());
  MOCK_CONST_METHOD0(current_year, unsigned());
  MOCK_CONST_METHOD0(time_steps, vector<string>&());
  MOCK_CONST_METHOD0(initialisation_phases, vector<string>&());
  MOCK_CONST_METHOD0(years, vector<unsigned>());
  MOCK_CONST_METHOD0(years_all, vector<unsigned>());
  MOCK_CONST_METHOD0(partition_type, PartitionType());
  MOCK_CONST_METHOD0(length_bins, vector<double>&());
  MOCK_CONST_METHOD0(length_plus, bool());
  MOCK_METHOD0(managers, shared_ptr<Managers>());
  MOCK_METHOD0(categories, niwa::Categories*());
  MOCK_METHOD0(objects, niwa::Objects&());

  /**
   * Have some setters to handle changing values on the model object
   */
  void set_run_mode(RunMode::Type run_mode) { run_mode_ = run_mode; }
  void set_min_age(unsigned min_age) { min_age_ = min_age; }
  void set_max_age(unsigned max_age) { max_age_ = max_age; }
  void set_start_year(unsigned start_year) { start_year_ = start_year; }
  void set_final_year(unsigned final_year) { final_year_ = final_year; }
  void set_projection_final_year(unsigned projection_final_year) { projection_final_year_ = projection_final_year; }
  void set_age_plus(bool age_plus) { age_plus_ = age_plus; }
  void set_length_plus(bool length_plus) { length_plus_ = length_plus; }
  void set_current_year(unsigned current_year) { current_year_ = current_year; }
  void set_time_steps(vector<string> time_steps) { time_steps_ = time_steps; }
  void set_initialiation_phases(vector<string> init_phases) { initialisation_phases_ = init_phases; }
  void set_partition_type(PartitionType partition_type) { partition_type_ = partition_type; }
  void set_length_bins(vector<double> length_bins) { length_bins_ = length_bins; }

  /**
   * Mock methods to call parent
   */
  vector<unsigned> mock_years() { return Model::years(); }
  vector<unsigned> mock_years_all() { return Model::years_all(); }
  unsigned         mock_age_spread() { return Model::age_spread(); }

  /**
   * Set our call with the values we've put on the base model class
   */
  void bind_calls() {
    EXPECT_CALL(*this, min_age()).WillRepeatedly(Return(min_age_));
    EXPECT_CALL(*this, max_age()).WillRepeatedly(Return(max_age_));
    EXPECT_CALL(*this, age_spread()).WillRepeatedly(Return(mock_age_spread()));
    EXPECT_CALL(*this, start_year()).WillRepeatedly(Return(start_year_));
    EXPECT_CALL(*this, final_year()).WillRepeatedly(Return(final_year_));
    EXPECT_CALL(*this, projection_final_year()).WillRepeatedly(Return(projection_final_year_));
    EXPECT_CALL(*this, current_year()).WillRepeatedly(Return(current_year_));
    EXPECT_CALL(*this, age_plus()).WillRepeatedly(Return(age_plus_));
    EXPECT_CALL(*this, time_steps()).WillRepeatedly(ReturnRef(time_steps_));
    EXPECT_CALL(*this, initialisation_phases()).WillRepeatedly(ReturnRef(initialisation_phases_));
    EXPECT_CALL(*this, years()).WillRepeatedly(Return(mock_years()));
    EXPECT_CALL(*this, years_all()).WillRepeatedly(Return(mock_years_all()));
    EXPECT_CALL(*this, partition_type()).WillRepeatedly(Return(partition_type_));
    EXPECT_CALL(*this, length_bins()).WillRepeatedly(ReturnRef(length_bins_));
    EXPECT_CALL(*this, length_plus()).WillRepeatedly(Return(length_plus_));
  }

  /**
   * This method will set all the calls to return default values
   * from GMock. Note: These are not the values specified on the class.
   *
   * boolean will default to false
   * Ints etc to 0
   * Pointers to null
   * Enums to null bytes
   */
  void bind_calls_to_default() {
    EXPECT_CALL(*this, min_age()).WillRepeatedly(DoDefault());
    EXPECT_CALL(*this, max_age()).WillRepeatedly(DoDefault());
    EXPECT_CALL(*this, age_spread()).WillRepeatedly(DoDefault());
    EXPECT_CALL(*this, start_year()).WillRepeatedly(DoDefault());
    EXPECT_CALL(*this, final_year()).WillRepeatedly(DoDefault());
    EXPECT_CALL(*this, projection_final_year()).WillRepeatedly(DoDefault());
    EXPECT_CALL(*this, current_year()).WillRepeatedly(DoDefault());
    EXPECT_CALL(*this, age_plus()).WillRepeatedly(DoDefault());
    EXPECT_CALL(*this, time_steps()).WillRepeatedly(DoDefault());
    EXPECT_CALL(*this, initialisation_phases()).WillRepeatedly(DoDefault());
    EXPECT_CALL(*this, years()).WillRepeatedly(DoDefault());
    EXPECT_CALL(*this, years_all()).WillRepeatedly(DoDefault());
    EXPECT_CALL(*this, partition_type()).WillRepeatedly(DoDefault());
    EXPECT_CALL(*this, length_bins()).WillRepeatedly(DoDefault());
    EXPECT_CALL(*this, length_plus()).WillRepeatedly(DoDefault());
    EXPECT_CALL(*this, managers()).WillRepeatedly(DoDefault());
    EXPECT_CALL(*this, categories()).WillRepeatedly(DoDefault());
    EXPECT_CALL(*this, objects()).WillRepeatedly(DoDefault());
  }
};
} /* namespace niwa */

#endif  // TESTMODE
#endif  /* MOCK_MODEL_H_ */
