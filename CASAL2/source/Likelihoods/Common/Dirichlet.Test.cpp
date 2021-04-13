//============================================================================
// Name        : Dirichlet.Test.cpp
// Copyright   : Copyright NIWA Science ©2009 - www.niwa.co.nz
//============================================================================
#ifdef TESTMODE

// Headers
#include "Dirichlet.h"

#include <iostream>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../Observations/Comparison.h"
#include "../../Utilities/RandomNumberGenerator.h"

// Namespaces
namespace niwa {
namespace likelihoods {

using std::cout;
using std::endl;
using observations::Comparison;

TEST(Likelihood, Dirichlet) {
  utilities::RandomNumberGenerator::Instance().Reset(31373u);

  Dirichlet likelihood(nullptr);
  map<unsigned, vector<Comparison> > comparison_list;

  // Test case 1
  Comparison comparison;
  comparison.age_             = 0;
  comparison.category_        = "A";
  comparison.expected_        = 0.1;
  comparison.observed_        = 0.25;
  comparison.error_value_     = 50;
  comparison.process_error_   = 1.0;
  comparison.delta_           = 1e-5;
  comparison_list[0].push_back(comparison);


  // Test case 2
  comparison.category_        = "A";
  comparison.expected_        = 0.2;
  comparison.observed_        = 0.25;
  comparison.error_value_     = 50;
  comparison.process_error_   = 1.0;
  comparison.delta_           = 1e-5;
  comparison_list[0].push_back(comparison);

  // Test case 3
  comparison.category_        = "A";
  comparison.expected_        = 0.3;
  comparison.observed_        = 0.25;
  comparison.error_value_     = 50;
  comparison.process_error_   = 1.0;
  comparison.delta_           = 1e-5;
  comparison_list[0].push_back(comparison);

  // Test case 4
  comparison.category_        = "A";
  comparison.expected_        = 0.4;
  comparison.observed_        = 0.25;
  comparison.error_value_     = 50;
  comparison.process_error_   = 1.0;
  comparison.delta_           = 1e-5;
  comparison_list[0].push_back(comparison);

  // Test case 5
  comparison.category_        = "B";
  comparison.expected_        = 0.5;
  comparison.observed_        = 0.4;
  comparison.error_value_     = 50;
  comparison.process_error_   = 0.1;
  comparison.delta_           = 1e-5;
  comparison_list[0].push_back(comparison);

  // Test case 6
  comparison.category_        = "B";
  comparison.expected_        = 0.5;
  comparison.observed_        = 0.6;
  comparison.error_value_     = 50;
  comparison.process_error_   = 0.1;
  comparison.delta_           = 1e-5;
  comparison_list[0].push_back(comparison);

  // Test case 7
  comparison.category_        = "C";
  comparison.expected_        = 1.0;
  comparison.observed_        = 0.1;
  comparison.error_value_     = 50;
  comparison.process_error_   = 0.5;
  comparison.delta_           = 1e-5;
  comparison_list[0].push_back(comparison);

  // Check initial score
  EXPECT_DOUBLE_EQ(-269.29109765110968, likelihood.GetInitialScore(comparison_list, 0));

  // Check Scores
  likelihood.GetScores(comparison_list);
  EXPECT_DOUBLE_EQ(  8.723231274827508, comparison_list[0][0].score_);
  EXPECT_DOUBLE_EQ( 25.278476730160975, comparison_list[0][1].score_);
  EXPECT_DOUBLE_EQ( 44.599342238420959, comparison_list[0][2].score_);
  EXPECT_DOUBLE_EQ( 65.679477048487513, comparison_list[0][3].score_);
  EXPECT_DOUBLE_EQ(  1.659118968284150, comparison_list[0][4].score_);
  EXPECT_DOUBLE_EQ(  1.050921306121904, comparison_list[0][5].score_);
  EXPECT_DOUBLE_EQ(110.046771629987430, comparison_list[0][6].score_);


  // check simulations
  likelihood.SimulateObserved(comparison_list);
  EXPECT_DOUBLE_EQ(0.06310697719429153,   comparison_list[0][0].observed_);
  EXPECT_DOUBLE_EQ(0.15200180984615036,   comparison_list[0][1].observed_);
  EXPECT_DOUBLE_EQ(0.47011274483757398,   comparison_list[0][2].observed_);
  EXPECT_DOUBLE_EQ(0.31477846812198418,   comparison_list[0][3].observed_);
  EXPECT_DOUBLE_EQ(0.41445498365152816,   comparison_list[0][4].observed_);
  EXPECT_DOUBLE_EQ(0.58554501634847189,   comparison_list[0][5].observed_);
  EXPECT_DOUBLE_EQ(1.00000000000000000,   comparison_list[0][6].observed_);
}

}
}

#endif
