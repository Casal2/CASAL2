/**
 * @file BinomialApprox.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 21/02/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// headers
#include "BinomialApprox.h"

#include <iostream>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Common/Likelihoods/Factory.h"
#include "Common/Observations/Comparison.h"
#include "Common/Utilities/RandomNumberGenerator.h"

// namespaces
namespace niwa {
namespace likelihoods {

using std::cout;
using std::endl;
using observations::Comparison;

TEST(Likelihood, BinomialApprox) {
  utilities::RandomNumberGenerator::Instance().Reset(31373u);

  BinomialApprox likelihood(nullptr);

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
  EXPECT_DOUBLE_EQ(-3.159984307040009,   comparison_list[0][0].score_);
  EXPECT_DOUBLE_EQ(-1.309802234588228,   comparison_list[0][1].score_);
  EXPECT_DOUBLE_EQ( 2.025569385058355,   comparison_list[0][2].score_);
  EXPECT_DOUBLE_EQ( 6.705430319465856,   comparison_list[0][3].score_);
  EXPECT_DOUBLE_EQ(13.350841316725985,   comparison_list[0][4].score_);
  EXPECT_DOUBLE_EQ( 0.91338771800667584, comparison_list[0][5].score_);
  EXPECT_DOUBLE_EQ( 1.776317543430387,   comparison_list[0][6].score_);

  // check simulations
  likelihood.SimulateObserved(comparison_list);
  EXPECT_DOUBLE_EQ(0.140000000000000, comparison_list[0][0].observed_);
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
