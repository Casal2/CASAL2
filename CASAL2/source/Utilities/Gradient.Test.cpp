/**
 * @file Gradient.Test.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2021-05-09
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifdef TESTMODE

// headers
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/algorithm/string/replace.hpp>
#include <iostream>

#include "../ConfigurationLoader/MPD.Mock.h"
#include "../DerivedQuantities/Manager.h"
#include "../MCMCs/MCMC.h"
#include "../MCMCs/Manager.h"
#include "../Model/Managers.h"
#include "../Model/Model.h"
#include "../ObjectiveFunction/ObjectiveFunction.h"
#include "../TestResources/MockClasses/Model.h"
#include "../TestResources/TestCases/TwoSexModel.h"
#include "../TestResources/TestFixtures/BaseThreaded.h"
#include "../TestResources/TestFixtures/BasicModel.h"
#include "Gradient.h"

// namespaces
namespace niwa::utilities::gradient {

using std::cout;
using std::endl;

// Empty class to give us better printing of Unit test results
class GradientThreadedModel : public testfixtures::BaseThreaded {};

/**
 *
 */
TEST_F(GradientThreadedModel, Calc_With_1_ModelThread) {
  AddConfigurationLine(testcases::test_cases_two_sex_model_population, __FILE__, 27);
  LoadConfiguration();

  ASSERT_NO_THROW(runner_->GoWithRunMode(RunMode::kTesting));
  ASSERT_EQ(1u, runner_->thread_pool()->threads().size());

  auto threads = runner_->thread_pool()->threads();
  ASSERT_NE(nullptr, threads[0]->model());
  ASSERT_EQ(PARAM_AGE, threads[0]->model()->type());

  vector<double> gradient_values;
  vector<double> estimate_values;
  vector<double> lower_bounds;
  vector<double> upper_bounds;
  double         step_size;
  double         last_score;

  // Do a simple check
  last_score      = 2698.1334195364388506;
  lower_bounds    = {1.0000000000000000364e-10, 100000, 1, 0.010000000000000000208};
  upper_bounds    = {0.10000000000000000555, 10000000000, 20, 50};
  step_size       = 9.9999999999999995475e-08;
  estimate_values = {0.00015313899999999915839, 997386.0000001770677, 8, 2.9999999999999995559};

  ASSERT_NO_THROW(gradient_values = gradient::Calculate(runner_->thread_pool(), estimate_values, lower_bounds, upper_bounds, step_size, last_score));
  ASSERT_EQ(4u, gradient_values.size());
  EXPECT_DOUBLE_EQ(19423.945915369863, gradient_values[0]);
  EXPECT_DOUBLE_EQ(64561.80283054358, gradient_values[1]);
  EXPECT_DOUBLE_EQ(-3577.4316210216889, gradient_values[2]);
  EXPECT_DOUBLE_EQ(-101.11005854788878, gradient_values[3]);
}

/**
 *
 */
TEST_F(GradientThreadedModel, Calc_With_6_ModelThreads) {
  string ammended_definition = testcases::test_cases_two_sex_model_population;
  boost::replace_all(ammended_definition, "threads 1", "threads 6");
  AddConfigurationLine(ammended_definition, __FILE__, 84);
  LoadConfiguration();

  ASSERT_NO_THROW(runner_->GoWithRunMode(RunMode::kTesting));
  ASSERT_EQ(6u, runner_->thread_pool()->threads().size());

  auto threads = runner_->thread_pool()->threads();
  ASSERT_NE(nullptr, threads[0]->model());
  ASSERT_EQ(PARAM_AGE, threads[0]->model()->type());

  vector<double> gradient_values;
  vector<double> estimate_values;
  vector<double> lower_bounds;
  vector<double> upper_bounds;
  double         step_size;
  double         last_score;

  // Do a simple check
  last_score      = 2698.1334195364388506;
  lower_bounds    = {1.0000000000000000364e-10, 100000, 1, 0.010000000000000000208};
  upper_bounds    = {0.10000000000000000555, 10000000000, 20, 50};
  step_size       = 9.9999999999999995475e-08;
  estimate_values = {0.00015313899999999915839, 997386.0000001770677, 8, 2.9999999999999995559};

  ASSERT_NO_THROW(gradient_values = gradient::Calculate(runner_->thread_pool(), estimate_values, lower_bounds, upper_bounds, step_size, last_score));
  ASSERT_EQ(4u, gradient_values.size());
  EXPECT_DOUBLE_EQ(19423.945915369863, gradient_values[0]);
  EXPECT_DOUBLE_EQ(64561.80283054358, gradient_values[1]);
  EXPECT_DOUBLE_EQ(-3577.4316210216889, gradient_values[2]);
  EXPECT_DOUBLE_EQ(-101.11005854788878, gradient_values[3]);
}

}  // namespace niwa::utilities::gradient
#endif
