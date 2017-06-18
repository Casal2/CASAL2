/**
 * @file Beta.Test.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 15/09/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 */
#ifdef TESTMODE

// Headers
#include "Beta.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <boost/lexical_cast.hpp>

#include "Model/Model.h"
#include "TestResources/MockClasses/Model.h"

// Namespaces
namespace niwa {
namespace additionalpriors {

using ::testing::Return;

/*
 * Mock class to make testing easier
 */
class MockBeta : public Beta {
public:
  MockBeta(Model* model, double mu, double sigma, double a, double b) : Beta(model) {
    mu_ = mu;
    sigma_ = sigma;
    a_ = a;
    b_ = b;
  }
};

/**
 * Test the results of our KnifeEdge are correct
 */
TEST(AdditionalPriors, Beta) {
  // layout is mu, sigma, a, b, expected_score
  vector<vector<double>> values = {
      { 4, 2, 2, 7, 0 },
      { 4, 2, 2, 7, 0 }
  };

  Model model;
  for (auto line : values) {
    MockBeta beta(&model, line[0], line[1], line[2], line[3]);
//    EXPECT_DOUBLE_EQ(line[4], beta.GetScore());
  }
}

} /* namespace additionalpriors */
} /* namespace niwa */

#endif /* ifdef TESTMODE */
