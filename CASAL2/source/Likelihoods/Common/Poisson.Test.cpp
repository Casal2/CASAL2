/**
 * @file Poisson.Test.cpp
 * @author  C. Marsh
 * @date 2023
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/�2023 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// headers
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>

#include "../../Likelihoods/Factory.h"
#include "../../Observations/Comparison.h"
#include "../../Utilities/RandomNumberGenerator.h"
#include "Poisson.h"

// namespaces
namespace niwa {
namespace likelihoods {

using observations::Comparison;
using std::cout;
using std::endl;

TEST(Likelihood, Poisson) {
  utilities::RandomNumberGenerator::Instance().Reset(31373u);

  Poisson likelihood(nullptr);

  map<unsigned, vector<Comparison> > comparison_list;

  // Test case 1
  Comparison comparison;
  comparison.category_      = "A";
  comparison.expected_      = 2.2;
  comparison.observed_      = 3;
  comparison.error_value_   = 3.2;
  comparison.process_error_ = 0.0;
  comparison.delta_         = 1e-5;
  comparison_list[0].push_back(comparison);

  // Test case 2
  Comparison comparison_B;
  comparison_B.category_    = "B";
  comparison_B.expected_    = 1.2;
  comparison_B.observed_    = 5;
  comparison_B.error_value_ = 1.2;
  comparison_B.process_error_ = 0.0;
  comparison_B.delta_         = 1e-5;
  comparison_list[1].push_back(comparison_B);

  // Test case 3
  Comparison comparison_C;
  comparison_C.category_    = "C";
  comparison_C.expected_    = 0.3;
  comparison_C.observed_    = 0;
  comparison_C.error_value_ = 0.3;
  comparison_C.process_error_ = 0.0;
  comparison_C.delta_         = 1e-5;
  comparison_list[2].push_back(comparison_C);


  // Check scores
  likelihood.GetScores(comparison_list);

  EXPECT_NEAR(1.626387, comparison_list[0][0].score_, 0.001) << " obs = " << comparison_list[0][0].observed_<< " exp = " << comparison_list[0][0].expected_; // -1.0 * dpois(3, 2.2, log = T)
  EXPECT_NEAR(5.075884, comparison_list[1][0].score_, 0.001) << " obs = " << comparison_list[1][0].observed_<< " exp = " << comparison_list[1][0].expected_; // -1.0 * dpois(5, 1.2, log = T)
  EXPECT_NEAR(0.3, comparison_list[2][0].score_, 0.001) << " obs = " << comparison_list[2][0].observed_<< " exp = " << comparison_list[2][0].expected_; // -1.0 * dpois(1, 0.3, log = T)

}

} /* namespace likelihoods */
} /* namespace niwa */

#endif /* TESTMODE */
