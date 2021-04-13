/**
 * @file Binomial.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 19/02/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// headers
#include "Binomial.h"

#include <iostream>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../Likelihoods/Common/Binomial.h"
#include "../../Likelihoods/Factory.h"
#include "../../Observations/Comparison.h"
#include "../../Utilities/RandomNumberGenerator.h"

// namespaces
namespace niwa {
namespace likelihoods {

using std::cout;
using std::endl;
using observations::Comparison;

TEST(Likelihood, Binomial) {
  utilities::RandomNumberGenerator::Instance().Reset(31373u);

  Binomial likelihood(nullptr);

  map<unsigned, vector<Comparison> > comparison_list;

  // Test case 1
  Comparison comparison;
  comparison.age_             = 0;
  comparison.category_        = "A";
  comparison.expected_        = 0.1;
  comparison.observed_        = 0.1;
  comparison.error_value_     = 50;
  comparison.process_error_   = 0.0;
  comparison.delta_           = 1e-5;
  comparison_list[0].push_back(comparison);

  // Test case 2
  comparison.category_        = "A";
  comparison.expected_        = 0.2;
  comparison.observed_        = 0.1;
  comparison.error_value_     = 50;
  comparison_list[0].push_back(comparison);

  // Test case 3
  comparison.category_        = "A";
  comparison.expected_        = 0.3;
  comparison.observed_        = 0.1;
  comparison.error_value_     = 50;
  comparison_list[0].push_back(comparison);

  // Test case 4
  comparison.category_        = "A";
  comparison.expected_        = 0.4;
  comparison.observed_        = 0.1;
  comparison.error_value_     = 50;
  comparison_list[0].push_back(comparison);

  // Test case 5
  comparison.category_        = "B";
  comparison.expected_        = 0.5;
  comparison.observed_        = 0.1;
  comparison.error_value_     = 50;
  comparison_list[0].push_back(comparison);

  // Test case 6
  comparison.category_        = "B";
  comparison.expected_        = 0.5;
  comparison.observed_        = 0.1;
  comparison.error_value_     = 50;
  comparison.process_error_   = 10;
  comparison_list[0].push_back(comparison);

  // Test case 7
  comparison.category_        = "C";
  comparison.expected_        = 0.5;
  comparison.observed_        = 0.1;
  comparison.error_value_     = 50;
  comparison.process_error_   = 15;
  comparison_list[0].push_back(comparison);

  // Check initial score
  EXPECT_DOUBLE_EQ(0.0, likelihood.GetInitialScore(comparison_list, 0));

  // Check scores
  likelihood.GetScores(comparison_list);
  EXPECT_DOUBLE_EQ( 1.687807099701715, comparison_list[0][0].score_);
  EXPECT_DOUBLE_EQ( 3.522307801439243, comparison_list[0][1].score_);
  EXPECT_DOUBLE_EQ( 7.503894929001944, comparison_list[0][2].score_);
  EXPECT_DOUBLE_EQ(13.002265158969662, comparison_list[0][3].score_);
  EXPECT_DOUBLE_EQ(20.091017458126572, comparison_list[0][4].score_);
  EXPECT_DOUBLE_EQ( 3.939720847325895, comparison_list[0][5].score_);
  EXPECT_DOUBLE_EQ( 5.256187812442846, comparison_list[0][6].score_);

  // check simulations
  likelihood.SimulateObserved(comparison_list);
  EXPECT_DOUBLE_EQ(0.14000000000000001, comparison_list[0][0].observed_);
  EXPECT_DOUBLE_EQ(0.28000000000000003, comparison_list[0][1].observed_);
  EXPECT_DOUBLE_EQ(0.26000000000000001, comparison_list[0][2].observed_);
  EXPECT_DOUBLE_EQ(0.5,                 comparison_list[0][3].observed_);
  EXPECT_DOUBLE_EQ(0.47999999999999998, comparison_list[0][4].observed_);
  EXPECT_DOUBLE_EQ(0.1111111111111111,  comparison_list[0][5].observed_);
  EXPECT_DOUBLE_EQ(0.41666666666666669, comparison_list[0][6].observed_);
}

} /* namespace likelihoods */
} /* namespace niwa */

#endif /* TESTMODE */
