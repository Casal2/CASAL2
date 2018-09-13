/**
 * @file LogNormal.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 21/02/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// headers
#include "LogNormal.h"

#include <iostream>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Likelihoods/Factory.h"
#include "Observations/Comparison.h"
#include "Utilities/RandomNumberGenerator.h"

// namespaces
namespace niwa {
namespace likelihoods {

using std::cout;
using std::endl;
using observations::Comparison;

TEST(Likelihood, LogNormal) {
  utilities::RandomNumberGenerator::Instance().Reset(31373u);

  LogNormal likelihood(nullptr);

  map<unsigned, vector<Comparison> > comparison_list;

  // Test case 1
  for (unsigned i = 0; i < 4; ++i) {
    Comparison comparison;
    comparison.age_             = 0;
    comparison.category_        = "A";
    comparison.expected_        = 0.25;
    comparison.observed_        = 0.25;
    comparison.error_value_     = 0.0001;
    comparison.process_error_   = 0.0;
    comparison.delta_           = 1e-5;
    comparison_list[0].push_back(comparison);
  }

  for (unsigned i = 0; i < 4; ++i) {
    Comparison comparison;
    comparison.age_             = 0;
    comparison.category_        = "B";
    comparison.expected_        = 0.25;
    comparison.observed_        = 0.25;
    comparison.error_value_     = 0.5;
    comparison.process_error_   = 0.0;
    comparison.delta_           = 1e-5;
    comparison_list[0].push_back(comparison);
  }

  // Check initial score
  EXPECT_DOUBLE_EQ(0.0, likelihood.GetInitialScore(comparison_list, 0));

  // Check scores
  likelihood.GetScores(comparison_list);
  EXPECT_DOUBLE_EQ(-9.2103403762649183,  comparison_list[0][0].score_);
  EXPECT_DOUBLE_EQ(-9.2103403762649183,  comparison_list[0][1].score_);
  EXPECT_DOUBLE_EQ(-9.2103403762649183,  comparison_list[0][2].score_);
  EXPECT_DOUBLE_EQ(-9.2103403762649183,  comparison_list[0][3].score_);
  EXPECT_DOUBLE_EQ(-0.72207704946548157, comparison_list[0][4].score_);
  EXPECT_DOUBLE_EQ(-0.72207704946548157, comparison_list[0][5].score_);
  EXPECT_DOUBLE_EQ(-0.72207704946548157, comparison_list[0][6].score_);
  EXPECT_DOUBLE_EQ(-0.72207704946548157, comparison_list[0][7].score_);

  // check simulations
  likelihood.SimulateObserved(comparison_list);
  EXPECT_DOUBLE_EQ(0.25001151803908866,   comparison_list[0][0].observed_);
  EXPECT_DOUBLE_EQ(0.25007450179597446,  comparison_list[0][1].observed_);
  EXPECT_DOUBLE_EQ(0.24998621490767847, comparison_list[0][2].observed_);
  EXPECT_DOUBLE_EQ(0.24998315947676214, comparison_list[0][3].observed_);
  EXPECT_DOUBLE_EQ(0.077455283489012747, comparison_list[0][4].observed_);
  EXPECT_DOUBLE_EQ(0.18175128229123977, comparison_list[0][5].observed_);
  EXPECT_DOUBLE_EQ(0.18460806280874451, comparison_list[0][6].observed_);
  EXPECT_DOUBLE_EQ(0.45561177771504574, comparison_list[0][7].observed_);
}

} /* namespace likelihoods */
} /* namespace niwa */

#endif /* TESTMODE */
