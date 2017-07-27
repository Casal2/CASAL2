/**
 * @file Normal.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 26/02/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// headers
#include "Normal.h"

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

TEST(Likelihood, Normal) {
  utilities::RandomNumberGenerator::Instance().Reset(31373u);

  Normal likelihood;

  map<unsigned, vector<Comparison> > comparison_list;

  // Test case 1
  Comparison comparison;
  comparison.age_             = 0;
  comparison.category_        = "A";
  comparison.expected_        = 500;
  comparison.observed_        = 1000;
  comparison.error_value_     = 0.25;
  comparison.process_error_   = 0.0;
  comparison.delta_           = 1e-11;
  comparison_list[0].push_back(comparison);

  // Test case 2
  comparison.category_        = "B";
  comparison.expected_        = 500;
  comparison.observed_        = 1000;
  comparison.error_value_     = 0.25;
  comparison.process_error_   = 0.20;
  comparison_list[0].push_back(comparison);

  // Test case 3
  comparison.category_        = "C";
  comparison.expected_        = 500;
  comparison.observed_        = 500;
  comparison.error_value_     = 0.25;
  comparison.process_error_   = 0.0;
  comparison_list[0].push_back(comparison);

  // Test case 4
  comparison.category_        = "D";
  comparison.expected_        = 500;
  comparison.observed_        = 500;
  comparison.error_value_     = 0.25;
  comparison.process_error_   = 0.20;
  comparison_list[0].push_back(comparison);

  // Check initial score
  EXPECT_DOUBLE_EQ(0.0, likelihood.GetInitialScore(comparison_list, 0));

  // Check scores
  likelihood.GetScores(comparison_list);
  EXPECT_DOUBLE_EQ(12.828313737302302,   comparison_list[0][0].score_);
  EXPECT_DOUBLE_EQ( 9.9537106387081593,  comparison_list[0][1].score_);
  EXPECT_DOUBLE_EQ( 4.8283137373023015,  comparison_list[0][2].score_);
  EXPECT_DOUBLE_EQ( 5.0756618582203545,  comparison_list[0][3].score_);

  // check simulations
  likelihood.SimulateObserved(comparison_list);
  EXPECT_DOUBLE_EQ(557.59511915082294, comparison_list[0][0].observed_);
  EXPECT_DOUBLE_EQ(976.98119489450221, comparison_list[0][1].observed_);
  EXPECT_DOUBLE_EQ(431.07888765290096, comparison_list[0][2].observed_);
  EXPECT_DOUBLE_EQ(392.17240873606607, comparison_list[0][3].observed_);
}

} /* namespace likelihoods */
} /* namespace niwa */

#endif /* TESTMODE */
