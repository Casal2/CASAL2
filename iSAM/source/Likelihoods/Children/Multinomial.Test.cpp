/**
 * @file Multinomial.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 13/02/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "Multinomial.h"

#include <iostream>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Likelihoods/Factory.h"
#include "Observations/Comparison.h"

// Namespaces
namespace isam {
namespace likelihoods {

using std::cout;
using std::endl;
using observations::Comparison;

TEST(Likelihood, Multinomial) {

  LikelihoodPtr likelihood = likelihoods::Factory::Create(PARAM_MULTINOMIAL);

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
  EXPECT_DOUBLE_EQ(-315.78552739959304, likelihood->GetInitialScore(comparison_list));

  // Check scores
  likelihood->GetScores(comparison_list);
  EXPECT_DOUBLE_EQ(16.300417207752272, comparison_list[0][0].score_);
  EXPECT_DOUBLE_EQ(12.834681304952547, comparison_list[0][1].score_);
  EXPECT_DOUBLE_EQ(10.807355764411724, comparison_list[0][2].score_);
  EXPECT_DOUBLE_EQ( 9.368945402152817, comparison_list[0][3].score_);
  EXPECT_DOUBLE_EQ( 8.253227645581770, comparison_list[0][4].score_);
  EXPECT_DOUBLE_EQ( 0.516444725003769, comparison_list[0][5].score_);
  EXPECT_DOUBLE_EQ( 0.872226985105489, comparison_list[0][6].score_);
}

}
}

#endif
