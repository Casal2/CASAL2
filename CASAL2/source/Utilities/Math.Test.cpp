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


/**
 * @brief
 * test cumulative distributions pnorm
 * the expected values are from R
 * R version 4.0.2 (2020-06-22)
 * Platform: x86_64-w64-mingw32/x64 (64-bit)
 * Running under: Windows 10 x64 (build 18363)
 * The near test shows expected precision, the old CASAL (pnorm) is not as accurate
 */
TEST(Math, pnorm) {
  // CASAL's pnorm
  EXPECT_DOUBLE_EQ(0.5, pnorm(23, 23, 3));
  EXPECT_NEAR(0.0001228664, pnorm(12, 23, 3), 0.001);
  EXPECT_NEAR(0.9995709, pnorm(33, 23, 3), 0.001);

  
  // CASAL2's pnorm // look at the expected precision
  EXPECT_NEAR(0.5, pnorm2(23, 23, 3), 0.000001);
  EXPECT_NEAR(0.0001228664, pnorm2(12, 23, 3), 0.000001);
  EXPECT_NEAR(0.9995709, pnorm2(33, 23, 3), 0.000001);

}


/**
 * @brief
 * test LnGamma call against R
 * the expected values are from R
 * R version 4.0.2 (2020-06-22)
 * Platform: x86_64-w64-mingw32/x64 (64-bit)
 * Running under: Windows 10 x64 (build 18363)
 * The near test shows expected precision, the old CASAL (pnorm) is not as accurate
 */
TEST(Math, LnGamma) {

  EXPECT_NEAR(48.47118, LnGamma(23), 0.0001);
  EXPECT_NEAR(19.98721, LnGamma(13), 0.0001);
  EXPECT_NEAR(6.213457, LnGamma(0.002), 0.0001);
  EXPECT_NEAR(15756.25, LnGamma(2333), 0.005); // not as precise for big values
}

}  // namespace niwa::utilities::math
#endif
