/**
 * @file Math.Test.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2021-06-10
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifdef TESTMODE

// headers
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>

#include "Math.h"

// namespaces
namespace niwa::utilities::math {

using std::cout;
using std::endl;

/**
 * @brief Construct a new test f object
 *
 */
TEST(Math, Test_UnScale_Penalties) {
  vector<double> scaled_values  = {-2875.635616544374443, -1400.8799037281319215, -0.051525859021434361473, -3.1571786457745338872};
  vector<double> lower_bounds   = {1.0000000000000000364e-10, 100000, 1, 0.010000000000000000208};
  vector<double> upper_bounds   = {0.10000000000000000555, 10000000000, 20, 50};
  double         unscaled_value = 0;

  ASSERT_NO_THROW(unscaled_value = unscale(scaled_values[0], lower_bounds[0], upper_bounds[0]));
  EXPECT_DOUBLE_EQ(1.1069300534419012e-05, unscaled_value);

  ASSERT_NO_THROW(unscaled_value = unscale(scaled_values[1], lower_bounds[1], upper_bounds[1]));
  EXPECT_DOUBLE_EQ(2372190.8435692932, unscaled_value);

  ASSERT_NO_THROW(unscaled_value = unscale(scaled_values[2], lower_bounds[2], upper_bounds[2]));
  EXPECT_DOUBLE_EQ(10.188652722538006, unscaled_value);

  ASSERT_NO_THROW(unscaled_value = unscale(scaled_values[3], lower_bounds[3], upper_bounds[3]));
  EXPECT_DOUBLE_EQ(4.8909677429617107, unscaled_value);
}

/**
 * @brief
 *
 */
TEST(Math, Test_UnscalingValue_Math) {
  vector<double> scaled_values  = {-3.1571787457745337235};
  vector<double> lower_bounds   = {0.010000000000000000208};
  vector<double> upper_bounds   = {50};
  double         unscaled_value = 0;

  ASSERT_NO_THROW(unscaled_value = unscale(scaled_values[0], lower_bounds[0], upper_bounds[0]));
  EXPECT_DOUBLE_EQ(4.8909675978793378803, unscaled_value);
  EXPECT_DOUBLE_EQ(4.8909675978793405449, unscaled_value);
}

}  // namespace niwa::utilities::math
#endif
