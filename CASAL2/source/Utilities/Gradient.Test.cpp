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
 * @brief Construct a new test f object
 *
 */
TEST(Gradient, Test_UnScale_Penalties) {
  vector<double> scaled_values  = {-2875.635616544374443, -1400.8799037281319215, -0.051525859021434361473, -3.1571786457745338872};
  vector<double> lower_bounds   = {1.0000000000000000364e-10, 100000, 1, 0.010000000000000000208};
  vector<double> upper_bounds   = {0.10000000000000000555, 10000000000, 20, 50};
  double         unscaled_value = 0;

  ASSERT_NO_THROW(unscaled_value = UnScaleValue(scaled_values[0], lower_bounds[0], upper_bounds[0]));
  EXPECT_DOUBLE_EQ(1.1069300534419012e-05, unscaled_value);

  ASSERT_NO_THROW(unscaled_value = UnScaleValue(scaled_values[1], lower_bounds[1], upper_bounds[1]));
  EXPECT_DOUBLE_EQ(2372190.8435692932, unscaled_value);

  ASSERT_NO_THROW(unscaled_value = UnScaleValue(scaled_values[2], lower_bounds[2], upper_bounds[2]));
  EXPECT_DOUBLE_EQ(10.188652722538006, unscaled_value);

  ASSERT_NO_THROW(unscaled_value = UnScaleValue(scaled_values[3], lower_bounds[3], upper_bounds[3]));
  EXPECT_DOUBLE_EQ(4.8909677429617107, unscaled_value);
}

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
  last_score      = 1993.8041770617783186;
  lower_bounds    = {1.0000000000000000364e-10, 100000, 1, 0.010000000000000000208};
  upper_bounds    = {0.10000000000000000555, 10000000000, 20, 50};
  step_size       = 1e-7;
  estimate_values = {1.106930053441901161e-05, 2372190.8435692931525, 10.18865272253800569, 4.890967742961707998};

  ASSERT_NO_THROW(gradient_values = gradient::Calculate(runner_->thread_pool(), estimate_values, lower_bounds, upper_bounds, step_size, last_score));
  ASSERT_EQ(4u, gradient_values.size());
  EXPECT_DOUBLE_EQ(-0.0062050367891151514063, gradient_values[0]);
  EXPECT_DOUBLE_EQ(-0.026270733620581847301, gradient_values[1]);
  EXPECT_DOUBLE_EQ(0.041659404813511033416, gradient_values[2]);
  EXPECT_DOUBLE_EQ(-0.0053182702773209664798, gradient_values[3]);
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
  last_score      = 1993.8041770617783186;
  lower_bounds    = {1.0000000000000000364e-10, 100000, 1, 0.010000000000000000208};
  upper_bounds    = {0.10000000000000000555, 10000000000, 20, 50};
  step_size       = 1e-7;
  estimate_values = {1.106930053441901161e-05, 2372190.8435692931525, 10.18865272253800569, 4.890967742961707998};

  ASSERT_NO_THROW(gradient_values = gradient::Calculate(runner_->thread_pool(), estimate_values, lower_bounds, upper_bounds, step_size, last_score));
  ASSERT_EQ(4u, gradient_values.size());
  EXPECT_DOUBLE_EQ(-0.0062050367891151514063, gradient_values[0]);
  EXPECT_DOUBLE_EQ(-0.026270733620581847301, gradient_values[1]);
  EXPECT_DOUBLE_EQ(0.041659404813511033416, gradient_values[2]);
  EXPECT_DOUBLE_EQ(-0.0053182702773209664798, gradient_values[3]);
}

/**
 * @brief Construct a new test f object
 *
 */
TEST_F(GradientThreadedModel, Calc_With_Scaled_Values) {
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
  last_score      = 1993.8041770617783186;
  lower_bounds    = {1.0000000000000000364e-10, 100000, 1, 0.010000000000000000208};
  upper_bounds    = {0.10000000000000000555, 10000000000, 20, 50};
  step_size       = 1e-7;
  estimate_values = {1.106930053441901161e-05, 2372190.8435692931525, 10.18865272253800569, 4.890967742961707998};

  ASSERT_NO_THROW(gradient_values = gradient::Calculate(runner_->thread_pool(), estimate_values, lower_bounds, upper_bounds, step_size, last_score));
  ASSERT_EQ(4u, gradient_values.size());
  EXPECT_DOUBLE_EQ(-0.0062050367891151514063, gradient_values[0]);
  EXPECT_DOUBLE_EQ(-0.026270733620581847301, gradient_values[1]);
  EXPECT_DOUBLE_EQ(0.041659404813511033416, gradient_values[2]);
  EXPECT_DOUBLE_EQ(-0.0053182702773209664798, gradient_values[3]);
}

}  // namespace niwa::utilities::gradient
#endif
