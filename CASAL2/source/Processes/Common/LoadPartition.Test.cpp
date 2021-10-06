/**
 * @file LoadPartition.Test.cpp
 * @author Scott Rasmussen (scott@zaita.com)
 * @brief Unit tests our LoadPartition process
 * @version 0.1
 * @date 2021-06-26
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifdef TESTMODE

// Headers
#include <iostream>

#include "../../Model/Factory.h"
#include "../../Model/Model.h"
#include "../../Partition/Partition.h"
#include "../../Runner.h"
#include "../../TestResources/TestFixtures/BaseThreaded.h"
#include "../../TimeSteps/Manager.h"
#include "LoadPartition.h"

// Namespaces
namespace niwa::processes {
using std::cout;
using std::endl;

// Empty class to give us better printing of Unit test results
class LoadPartitionModel : public testfixtures::BaseThreaded {};

string basic_definition = R"(
@model
type age
min_age 1
max_age 10
start_year 2000
final_year 2001
time_steps step_one

@categories
format sex
names male female
age_lengths no_age_length*2

@age_length no_age_length
type none
length_weight no_length_weight

@length_weight no_length_weight
type none

@time_steps step_one
processes load_partition

@process load_partition
type load_partition
table data
category 1 2 3 4 5 6 7 8 9 10
male 100 200 300 400 500 600 700 800 900 1000
female 50 100 150 200 250 300 350 400 450 500
end_table
)";

/**
 *
 */
TEST_F(LoadPartitionModel, BasicImplementation) {
  AddConfigurationLine(basic_definition, __FILE__, __LINE__);
  ASSERT_NO_THROW(LoadConfiguration());

  ASSERT_NO_THROW(runner_->GoWithRunMode(RunMode::kBasic));
  auto  model     = runner_->model();
  auto& partition = model->partition();

  auto& male = partition.category("male");
  auto& data = male.data_;
  EXPECT_DOUBLE_EQ(100, data[0]);
  EXPECT_DOUBLE_EQ(200, data[1]);
  EXPECT_DOUBLE_EQ(300, data[2]);
  EXPECT_DOUBLE_EQ(400, data[3]);
  EXPECT_DOUBLE_EQ(500, data[4]);
  EXPECT_DOUBLE_EQ(600, data[5]);
  EXPECT_DOUBLE_EQ(700, data[6]);
  EXPECT_DOUBLE_EQ(800, data[7]);
  EXPECT_DOUBLE_EQ(900, data[8]);
  EXPECT_DOUBLE_EQ(1000, data[9]);

  auto& female = partition.category("female");
  auto& fdata  = female.data_;
  EXPECT_DOUBLE_EQ(50, fdata[0]);
  EXPECT_DOUBLE_EQ(100, fdata[1]);
  EXPECT_DOUBLE_EQ(150, fdata[2]);
  EXPECT_DOUBLE_EQ(200, fdata[3]);
  EXPECT_DOUBLE_EQ(250, fdata[4]);
  EXPECT_DOUBLE_EQ(300, fdata[5]);
  EXPECT_DOUBLE_EQ(350, fdata[6]);
  EXPECT_DOUBLE_EQ(400, fdata[7]);
  EXPECT_DOUBLE_EQ(450, fdata[8]);
  EXPECT_DOUBLE_EQ(500, fdata[9]);
}

/**
 * @brief
 *
 */
string reversed_definition = R"(
@model
type age
min_age 1
max_age 10
start_year 2000
final_year 2001
time_steps step_one

@categories
format sex
names male female
age_lengths no_age_length*2

@age_length no_age_length
type none
length_weight no_length_weight

@length_weight no_length_weight
type none

@time_steps step_one
processes load_partition

@process load_partition
type load_partition
table data
category 10 9 8 7 6 5 4 3 2 1
male 100 200 300 400 500 600 700 800 900 1000
female 50 100 150 200 250 300 350 400 450 500
end_table
)";

/**
 *
 */
TEST_F(LoadPartitionModel, ReversedImplementation) {
  AddConfigurationLine(reversed_definition, __FILE__, __LINE__);
  ASSERT_NO_THROW(LoadConfiguration());

  ASSERT_NO_THROW(runner_->GoWithRunMode(RunMode::kBasic));
  auto  model     = runner_->model();
  auto& partition = model->partition();

  auto& male = partition.category("male");
  auto  data = male.data_;
  EXPECT_DOUBLE_EQ(1000, data[0]);
  EXPECT_DOUBLE_EQ(900, data[1]);
  EXPECT_DOUBLE_EQ(800, data[2]);
  EXPECT_DOUBLE_EQ(700, data[3]);
  EXPECT_DOUBLE_EQ(600, data[4]);
  EXPECT_DOUBLE_EQ(500, data[5]);
  EXPECT_DOUBLE_EQ(400, data[6]);
  EXPECT_DOUBLE_EQ(300, data[7]);
  EXPECT_DOUBLE_EQ(200, data[8]);
  EXPECT_DOUBLE_EQ(100, data[9]);
}

/**
 * @brief
 *
 */
string wonky_definition = R"(
@model
type age
min_age 1
max_age 10
start_year 2000
final_year 2001
time_steps step_one

@categories
format sex
names male female
age_lengths no_age_length*2

@age_length no_age_length
type none
length_weight no_length_weight

@length_weight no_length_weight
type none

@time_steps step_one
processes load_partition

@process load_partition
type load_partition
table data
category 1 3 5 7 9 2 4 6 8 10
male 100 200 300 400 500 600 700 800 900 1000
female 50 100 150 200 250 300 350 400 450 500
end_table
)";

/**
 *
 */
TEST_F(LoadPartitionModel, WonkyImplementation) {
  AddConfigurationLine(wonky_definition, __FILE__, __LINE__);
  ASSERT_NO_THROW(LoadConfiguration());

  ASSERT_NO_THROW(runner_->GoWithRunMode(RunMode::kBasic));
  auto  model     = runner_->model();
  auto& partition = model->partition();

  auto& male = partition.category("male");
  auto  data = male.data_;
  EXPECT_DOUBLE_EQ(100, data[0]);
  EXPECT_DOUBLE_EQ(600, data[1]);
  EXPECT_DOUBLE_EQ(200, data[2]);
  EXPECT_DOUBLE_EQ(700, data[3]);
  EXPECT_DOUBLE_EQ(300, data[4]);
  EXPECT_DOUBLE_EQ(800, data[5]);
  EXPECT_DOUBLE_EQ(400, data[6]);
  EXPECT_DOUBLE_EQ(900, data[7]);
  EXPECT_DOUBLE_EQ(500, data[8]);
  EXPECT_DOUBLE_EQ(1000, data[9]);
}

}  // namespace niwa::processes

#endif /* TESTMODE */
