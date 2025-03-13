/**
 * @file Bernoulli.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2025/03/13
 * @section LICENSE
 *
 * Copyright Casal2 Project 2025 - https://github.com/Casal2/
 *
 */
#ifdef TESTMODE

// headers
#include "Bernoulli.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>

#include "../../Likelihoods/Common/Binomial.h"
#include "../../Likelihoods/Factory.h"
#include "../../Observations/Comparison.h"
#include "../../Utilities/RandomNumberGenerator.h"

// namespaces
namespace niwa::likelihoods {

using observations::Comparison;
using std::cout;
using std::endl;

TEST(Likelihood, Bernoulli) {
  utilities::RandomNumberGenerator::Instance().Reset(31373u);

  Bernoulli likelihood(nullptr);

  map<unsigned, vector<Comparison> > comparison_list;

  // Test case 1
  Comparison comparison;
  comparison.age_           = 0;
  comparison.category_      = "A";
  comparison.expected_      = 0.1;
  comparison.observed_      = 1.0;
  comparison.error_value_   = 50;
  comparison.process_error_ = 0.0;
  comparison.delta_         = 1e-5;
  comparison_list[0].push_back(comparison);

  // Test case 2
  comparison.category_    = "A";
  comparison.expected_    = 0.2;
  comparison.observed_    = 1.0;
  comparison.error_value_ = 20;
  comparison_list[0].push_back(comparison);

  // Test case 3
  comparison.category_    = "A";
  comparison.expected_    = 0.25;
  comparison.observed_    = 1.0;
  comparison.error_value_ = 40;
  comparison_list[0].push_back(comparison);

  // Test case 4
  comparison.category_    = "A";
  comparison.expected_    = 0.3;
  comparison.observed_    = 0.0;
  comparison.error_value_ = 50;
  comparison_list[0].push_back(comparison);

  // Test case 5
  comparison.category_    = "B";
  comparison.expected_    = 0.4;
  comparison.observed_    = 1.0;
  comparison.error_value_ = 50;
  comparison_list[0].push_back(comparison);

  // Test case 6
  comparison.category_      = "B";
  comparison.expected_      = 0.5;
  comparison.observed_      = 0.0;
  comparison.error_value_   = 50;
  comparison.process_error_ = 10;
  comparison_list[0].push_back(comparison);

  // Test case 7
  comparison.category_      = "C";
  comparison.expected_      = 0.65;
  comparison.observed_      = 0.0;
  comparison.error_value_   = 50;
  comparison.process_error_ = 15;
  comparison_list[0].push_back(comparison);

  // Check initial score
  EXPECT_DOUBLE_EQ(0.0, likelihood.GetInitialScore(comparison_list, 0));

  // Check scores
  likelihood.GetScores(comparison_list);
  EXPECT_DOUBLE_EQ(2.3025850929940455, comparison_list[0][0].score_);
  EXPECT_DOUBLE_EQ(1.6094379124341003, comparison_list[0][1].score_);
  EXPECT_DOUBLE_EQ(1.3862943611198906, comparison_list[0][2].score_);
  EXPECT_DOUBLE_EQ(0.35667494393873245, comparison_list[0][3].score_);
  EXPECT_DOUBLE_EQ(0.916290731874155, comparison_list[0][4].score_);
  EXPECT_DOUBLE_EQ(0.69314718055994529, comparison_list[0][5].score_);
  EXPECT_DOUBLE_EQ(1.0498221244986778, comparison_list[0][6].score_);

  // check simulations
  likelihood.SimulateObserved(comparison_list);
  EXPECT_DOUBLE_EQ(0.0, comparison_list[0][0].observed_);
  EXPECT_DOUBLE_EQ(0.0, comparison_list[0][1].observed_);
  EXPECT_DOUBLE_EQ(0.0, comparison_list[0][2].observed_);
  EXPECT_DOUBLE_EQ(1.0, comparison_list[0][3].observed_);
  EXPECT_DOUBLE_EQ(1.0, comparison_list[0][4].observed_);
  EXPECT_DOUBLE_EQ(1.0, comparison_list[0][5].observed_);
  EXPECT_DOUBLE_EQ(0.0, comparison_list[0][6].observed_);
}

} /* namespace niwa::likelihoods */

#endif /* TESTMODE */
